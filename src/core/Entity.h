#ifndef ENTITY_H
#define ENTITY_H

#include <optional>
#include <stdexcept>
#include <string>
#include <memory>
#include <unordered_map>
#include <utility> // for std::move
#include <vector>
#include <algorithm> // for std::reverse
#include <refl.hpp>
#include "Property.h"
#include "PathUtils.h"
#include "StringIntern.h"
#include "TypeInfo.h"

template<typename T>
class Visitor;

class Entity : public std::enable_shared_from_this<Entity> {
public:
    using ObjectId = uint32_t;
    using EntityId = uint32_t;
    using EntityPtr = std::shared_ptr<Entity>;
    using ConstEntityPtr = std::shared_ptr<const Entity>;

    explicit Entity(const std::string& name, std::uint32_t hash) : name_(name), hash_(hash) {}

    const std::string& getName() const { return name_; }

    void addChild(EntityPtr child) {
        children_[child->getName()] = child;
        child->setParent(shared_from_this());
    }

    EntityPtr getChild(const std::string& name) const {
        auto it = children_.find(name);
        if (it != children_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void setParent(EntityPtr parent) { parent_ = parent; }
    EntityPtr getParent() const { return parent_.lock(); }

    std::string getPath() const {
        std::vector<std::string> path_parts;
        for (auto* current = this; current != nullptr; current = current->getParent().get()) {
            path_parts.push_back(current->getName());
        }
        std::reverse(path_parts.begin(), path_parts.end());
        return PathUtils::join(path_parts, "/");
    }

    std::unordered_map<StringRef, std::shared_ptr<void>> getObjects() const {
        return objects_;
    }

    // TODO: object check is storaged
    template <typename T>
    void setObject(const std::shared_ptr<T>& object) {
        auto type_name = TypeInfo::getTypeName<T>();
        objects_[type_name] = std::static_pointer_cast<void>(object);
    }

    template <typename T>
    std::shared_ptr<T> getObject() {
        auto type_name = TypeInfo::getTypeName<T>();
        auto it = objects_.find(type_name);
        if (it != objects_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        auto it2 = objectsInSerialize_.find(type_name);
        if (it2 != objectsInSerialize_.end()) {
            deserialize(type_name);
            auto obj = objects_[type_name];
            return std::static_pointer_cast<T>(obj); 
        }
        return nullptr;
    }

    template <typename T, typename ObjType>
    std::shared_ptr<T> getOrCreateObject(std::shared_ptr<ObjType> generic) {
        auto obj = getObject<T>();
        if (obj) return obj;
        auto type_name = TypeInfo::getTypeName<T>();

        return std::static_pointer_cast<T>(getOrCreateObject(type_name, std::static_pointer_cast<void>(generic)));
    }

    template <typename T>
    std::shared_ptr<T> getOrCreateObject(std::optional<GenericRef> rfl_generic) {
        auto obj = getObject<T>();
        if (obj) return obj;
        auto type_name = TypeInfo::getTypeName<T>();

        return std::static_pointer_cast<T>(getOrCreateObject(type_name, rfl_generic));
    }


    // TODO: object check is storaged
    void setObject(StringRef type_name, std::shared_ptr<void> object) {
        objects_[type_name] = object; 
    }
    std::shared_ptr<void> getObject(StringRef type_name) const {
        auto it = objects_.find(type_name);
        return (it != objects_.end()) ? it->second : nullptr;
    }

    std::shared_ptr<void> getOrCreateObject(StringRef type_name, std::optional<GenericRef> rfl_generic);
    std::shared_ptr<void> getOrCreateObject(StringRef type_name, std::shared_ptr<void> generic);

    const auto& getChildren() const { return children_; }

    const auto& getProperties() const {
        return properties_;
    }

    ValueType getProperty(const std::string& type_name, const std::string& member_name) const {
        auto& type_property = properties_.at(type_name);
        return type_property.at(member_name);
    }

    void setProperty(StringRef type_name, const std::string& member_name, ValueType value) {
        properties_[type_name][member_name] = value;
    }

    // TODO : deserialize in this step, and remove objectsInSerialize_ member
    //      the serialiable can be access from TypeManager
    void setSerialize(StringRef type_name, const std::string& value_str) {
        objectsInSerialize_[type_name] = rfl::json::read<rfl::Generic>(value_str).value();
    }

    void setSerialize(StringRef type_name, ValueType value) {
        objectsInSerialize_[type_name] = std::get<rfl::Generic>(value);
    }

    void deserialize(StringRef type_name) ;

    rfl::Generic getSerialize(StringRef type_name) {
        return objectsInSerialize_.at(type_name);
    }

    const auto& getSerializies() const {
        return objectsInSerialize_;
    }

    void accept(Visitor<void>& visitor, int level = 0) const ;

    EntityPtr findEntity(const std::string& path) const {
        auto parts = PathUtils::split(path);
        auto current_entity = std::const_pointer_cast<Entity>(shared_from_this());

        for (const auto& part : parts) {
            auto child = current_entity->getChild(part);
            if (!child) {
                return nullptr;
            }
            current_entity = child;
        }

        return current_entity;
    }

    std::uint32_t getHash() { return hash_;}

private:
    std::string name_;
    std::uint32_t hash_;
    std::unordered_map<std::string, EntityPtr> children_;
    std::unordered_map<StringRef, std::shared_ptr<void>> objects_;
    std::unordered_map<StringRef, ElementProperties> properties_;
    std::unordered_map<StringRef, rfl::Generic> objectsInSerialize_;
    std::weak_ptr<Entity> parent_;
    StringRef type_name_;
friend class EntityRef;
};

#endif // ENTITY_H
