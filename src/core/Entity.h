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
#include "Object.h"

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

    EntityPtr getOrCreateChild(const std::string& name) ;

    EntityPtr getChild(const std::string& name) const {
        auto it = children_.find(name);
        if (it != children_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void setParent(EntityPtr parent) { parent_ = parent; }
    EntityPtr getParent() const { return parent_.lock(); }

    std::string getEntityPath() const {
        std::vector<std::string> path_parts;
        for (auto* current = this; current != nullptr; current = current->getParent().get()) {
            path_parts.push_back(current->getName());
        }
        std::reverse(path_parts.begin(), path_parts.end());
        return PathUtils::join(path_parts, "/");
    }

    auto getObjects() const {
        return objects_;
    }

    // TODO: object check is storaged
    template <typename T>
    void setObject(ObjRef object) {
        auto type_name = TypeInfo::getTypeName<T>();
        objects_[type_name] = object;
    }

    template <typename T>
    std::optional<ObjRef> getObject() {
        auto type_name = TypeInfo::getTypeName<T>();
        auto it = objects_.find(type_name);
        if (it != objects_.end()) {
            return it->second;
        }
        auto it2 = objectsInSerialize_.find(type_name);
        if (it2 != objectsInSerialize_.end()) {
            deserialize(type_name);
            auto obj = objects_.at(type_name);
            return obj; 
        }
        return std::nullopt;
    }

    template <typename T, typename ObjType>
    ObjRef getOrCreateObject(std::shared_ptr<ObjType> generic) {
        auto obj = getObject<T>();
        if (obj) return obj.value();
        auto type_name = TypeInfo::getTypeName<T>();

        return getOrCreateObject(type_name, std::static_pointer_cast<void>(generic));
    }

    template <typename T>
    ObjRef getOrCreateObject(GenericRef rfl_generic) {
        auto obj = getObject<T>();
        if (obj) return obj.value();
        auto type_name = TypeInfo::getTypeName<T>();

        return getOrCreateObject(type_name, rfl_generic);
    }

    template <typename T>
    ObjRef getOrCreateObject() {
        auto obj = getObject<T>();
        if (obj) return obj.value();
        auto type_name = TypeInfo::getTypeName<T>();

        return getOrCreateObject(type_name);
    }

    template <typename T>
    void removeObject() {
        auto type_name = TypeInfo::getTypeName<T>();
        removeObject(type_name);
    }

    void removeObject(StringRef type_name) ;

    // TODO: object check is storaged
    void setObject(StringRef type_name, ObjRef object) {
        objects_.emplace(type_name,  object);
    }
    std::optional<ObjRef> getObject(StringRef type_name) const {
        auto it = objects_.find(type_name);
        if (it != objects_.end()) {
            //std::make_optional(it->second);
            return it->second;;
        } else {
            return std::nullopt;
        }
    }

    ObjRef getOrCreateObject(StringRef type_name);
    ObjRef getOrCreateObject(StringRef type_name, GenericRef rfl_generic);
    ObjRef getOrCreateObject(StringRef type_name, std::shared_ptr<void> generic);

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
        auto it = objectsInSerialize_.find(type_name);
        if (it != objectsInSerialize_.end()) {
            std::cout << "Warning: overriding json str at setSerialize";
            objectsInSerialize_[type_name] = rfl::json::read<rfl::Generic>(value_str).value();
        } else {
            objectsInSerialize_.emplace(type_name, rfl::json::read<rfl::Generic>(value_str).value());
        }
    }

    void setSerialize(StringRef type_name, ValueType value) {
        auto it = objectsInSerialize_.find(type_name);
        if (it != objectsInSerialize_.end()) {
            std::cout << "Warning: overriding json str at setSerialize";
            objectsInSerialize_[type_name] = std::get<rfl::Generic>(value);
        } else {
            objectsInSerialize_.emplace(type_name, std::get<rfl::Generic>(value));
        }
    }

    void deserialize(StringRef type_name) ;

    rfl::Generic getSerialize(StringRef type_name) {
        return objectsInSerialize_.at(type_name);
    }

    const auto& getSerializies() const {
        return objectsInSerialize_;
    }

    template<typename T = void>
    void accept(Visitor<T>& visitor, int level = 0) const {
        visitor.visit(shared_from_this(), level);
    }

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

    EntityId getHash() { return hash_;}

private:
    std::string name_;
    std::uint32_t hash_;
    std::unordered_map<std::string, EntityPtr> children_;
    std::unordered_map<StringRef, ObjRef> objects_;
    std::unordered_map<StringRef, ElementProperties> properties_;
    std::unordered_map<StringRef, rfl::Generic> objectsInSerialize_;
    std::weak_ptr<Entity> parent_;
friend class EntityRef;
};

#endif // ENTITY_H
