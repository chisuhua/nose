#ifndef TYPEMANAGER_H
#define TYPEMANAGER_H
#include <stdexcept>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <variant>
#include <any>
#include <type_traits>
#include <refl.hpp>
#include "EntityIntern.h"
#include "StringIntern.h"
#include "Property.h"
#include "TypeInfo.h"

using KeyValueParser = ValueType(*)(const std::string&, const std::string&);
using PropertiesSetter = void(*)(const std::shared_ptr<void>, ElementProperties&);

using ObjectCreator = std::function<std::shared_ptr<void>(std::optional<GenericRef>)>;

using StorageObjectCreator = std::function<std::shared_ptr<void>(EntityRef, std::optional<GenericRef>)>;


class PropertyMeta {
public:

    template <typename T>
    static PropertyMeta createMetadata()
    {
        PropertyMeta md;
        if constexpr(refl::trait::is_reflectable_v<T>) {
            constexpr auto type_name = refl::reflect<T>().name;
            md.name_ = type_name.c_str();
            md.value_parser_ = &PropertyMeta::ParsePropertyValue<T>;
            md.properties_setter_ = &PropertyMeta::SetProperties<T>;
            md.object_creator_ = &PropertyMeta::CreateObject<T>;
            md.storage_object_creator_ = nullptr;
        } else {
            auto type_name = TypeInfo::Get<T>().Name();
            md.name_ = type_name;
            md.value_parser_ = &PropertyMeta::ParsePropertyValue<T>;
            md.properties_setter_ = &PropertyMeta::SetProperties<T>;
            md.object_creator_ = &PropertyMeta::CreateObject<T>;
            md.storage_object_creator_ = nullptr;
        }

        return md;
    }

    ValueType parsePropertyValue(const std::string& path_key, const std::string& value) const {
        return value_parser_(path_key, value);
    }

    void setProperties(std::shared_ptr<void> instance, ElementProperties& properties) {
        return properties_setter_(instance, properties);
    }

    std::shared_ptr<void> createObject(std::optional<GenericRef> rfl_generic) {
        return object_creator_(rfl_generic);
    }

    std::shared_ptr<void> createStorageObject(EntityRef entity, std::optional<GenericRef> rfl_generic) {
        if (storage_object_creator_ == nullptr) {
            throw std::runtime_error("please register storage object creator before use it\n");
        }
        return storage_object_creator_(entity, rfl_generic);
    }

    void registerStorageObjectCreator(StorageObjectCreator& creator) {
        storage_object_creator_ = creator;
    }

    StringRef name() const {
        return name_;
    }

private:
    StringRef name_;
    KeyValueParser value_parser_;
    PropertiesSetter properties_setter_;
    ObjectCreator object_creator_;
    StorageObjectCreator storage_object_creator_;

    template <typename T>
    static ValueType ParsePropertyValue(const std::string& path_key, const std::string& value) {
        ValueType result;
        if constexpr(refl::trait::is_reflectable_v<T>) {
            for_each(refl::reflect<T>().members, [&](auto member) {
                using member_type = typename decltype(member)::value_type;
                if ((member.name.str() != path_key) && (path_key != "*")) {
                    //std::cout << "skip parse value: " << path_key << "=" << value << " member name:" << member.name.str() << " member type:" << TypeInfo::Get<member_type>().Name()->str() << "\n";
                    return;
                }
                if constexpr (refl::descriptor::has_attribute<Property>(member)) {
                    const auto& prop = refl::descriptor::get_attribute<Property>(member);
                    std::cout << "ParseProperty Value: " << path_key << "=" << value << "\n";
                    result = prop.template parse<member_type>(value);
                } else {
                    std::cout << "ParseNonProperty Value: " << path_key << "=" << value << " member name:" << member.name.str() << " member type:" << TypeInfo::Get<member_type>().Name()->str() << "\n";
                    result = parser::default_parser<member_type>(value);
                }
            });
        } else {
            throw std::runtime_error(" non reflectable should not called ParsePropertyValue");
        }
        return result;
    }

    template <typename T>
    static void SetProperties(const std::shared_ptr<void> instance, ElementProperties& properties) {
        if constexpr(refl::trait::is_reflectable_v<T>) {
            auto instanceT = std::static_pointer_cast<T>(instance);
            for_each(refl::reflect<T>().members, [&](auto member) {
                if constexpr (refl::descriptor::has_attribute<Property>(member)) {
                    auto&& prop = refl::descriptor::get_attribute<Property>(member);
                    if (auto propIter = properties.find(member.name.str()); propIter != properties.end()) {
                        using member_type = typename decltype(member)::value_type;
                        member(*instanceT) = std::visit([&](auto&& arg) -> member_type {
                            if constexpr (std::is_same_v<std::decay_t<member_type>, std::decay_t<decltype(arg)>>) {
                                std::cout << "SetProperty Value: " << arg << "\n";
                                return arg;
                            } else if constexpr (std::is_same_v<rfl::Generic, std::decay_t<decltype(arg)>>) {
                                std::cout << "SetProperty json Value: " << rfl::json::write(arg) << "\n";
                                return rfl::from_generic<member_type>(arg).value();
                            } else if constexpr (std::is_same_v<std::any, std::decay_t<decltype(arg)>>) {
                                std::cout << "SetProperty any Value: " << member.name.str() <<"\n";
                                return std::any_cast<member_type>(arg);
                            } else {
                                std::cout << "member name:" << member.name.str() << " member type:" << TypeInfo::Get<member_type>().Name()->str() << " and arg: " <<  " arg type " + TypeInfo::Get<decltype(arg)>().Name()->str() << "\n";
                                //return std::any_cast<member_type>(arg);
                                throw std::runtime_error("Unsupported property type conversion");
                            }
                        }, propIter->second);
                    }
                } else {
                    if (auto propIter = properties.find(member.name.str()); propIter != properties.end()) {
                        using member_type = typename decltype(member)::value_type;
                        member(*instanceT) = std::visit([&](auto&& arg) -> member_type {
                            if constexpr (std::is_same_v<std::decay_t<member_type>, std::decay_t<decltype(arg)>>) {
                                std::cout << "SetNonProperty Value: " << arg << "\n";
                                return arg;
                            } else if constexpr (std::is_same_v<rfl::Generic,std::decay_t<decltype(arg)>>) {
                                std::cout << "SetNonProperty json Value: " << rfl::json::write(arg) << "\n";
                                return rfl::from_generic<member_type>(arg).value();
                            } else if constexpr (std::is_same_v<std::any, std::decay_t<decltype(arg)>>) {
                                std::cout << "SetNonProperty any Value: " << member.name.str()  << "\n";
                                return std::any_cast<member_type>(arg);
                            } else {
                                // return std::any_cast<member_type>(arg);
                                //return member_type(arg);
                                std::cout << "member name:" << member.name.str() << " member type:" << TypeInfo::Get<member_type>().Name()->str() << " and arg: " <<  " arg type " + TypeInfo::Get<decltype(arg)>().Name()->str() << "\n";
                                //throw std::runtime_error(TypeInfo::Get<member_type>().Name()->str() + " and arg type " + TypeInfo::Get<decltype(arg)>().Name()->str());
                                throw std::runtime_error("Unsupported nonproperty type conversion");
                            }
                        }, propIter->second);
                    }
                }
            });
        } else {
            throw std::runtime_error(" non reflectable should not called SetProperties") ;
        }
    }

    template <typename T>
    static std::shared_ptr<void> CreateObject(std::optional<GenericRef> rfl_generic) {
        if constexpr(has_generic_v<T>) {
            using GenericType = typename T::GenericType;
            if (rfl_generic) {
                GenericType obj = rfl::from_generic<GenericType>(rfl_generic.value()).value();
                return std::static_pointer_cast<void>(std::make_shared<T>(obj));
            } else {
                GenericType obj;
                return std::static_pointer_cast<void>(std::make_shared<T>(obj));
            }
        } else {
            T obj = rfl::from_generic<T>(rfl_generic.value()).value();
            return std::static_pointer_cast<void>(std::make_shared<T>(obj));
        }
    }

    template <typename T>
    static std::shared_ptr<T> Cast(std::optional<GenericRef> rfl_generic) {
    }
};



class TypeManager {
public:
    static TypeManager& instance() {
        static TypeManager instance;
        return instance;
    }

    TypeManager() = default;

    ~TypeManager() {};

    template <typename T>
    StringRef getTypeName() {
        auto& ti = TypeInfo::Get<T>();
        return ti.Name();
    }

    template <typename T>
    StringRef registerType() {
        StringRef type_name = getTypeName<T>();
        auto it = metadata.find(type_name);
        if (it == metadata.end()) {
            metadata[type_name] = PropertyMeta::createMetadata<T>();
            if constexpr(has_generic_v<T>) {
                using GenericType = typename T::GenericType;
                StringRef generic_type_name = getTypeName<GenericType>();
                metadata[generic_type_name] = PropertyMeta::createMetadata<GenericType>();
            }
        }
        return type_name;
    }

    //template <template<typename> typename Template, typename Arg>
    //void registerTemplateType() {
        //constexpr auto type = refl::reflect<Template<Arg>>>();
        //metadata[type.name] = PropertyMeta::createMetadata<Template<Arg>>(type.name);
        ////factories_[typeName] = []() -> std::shared_ptr<void> {
            ////return std::make_shared<Template<Arg>>();
        ////};
        ////typeDescriptors_[typeName] = &refl::reflect<Template<Arg>>();
    //}

    const std::unordered_map<StringRef, PropertyMeta>& getPropertyMeta() const {
        return metadata;
    }


    ValueType parsePropertyValue(StringRef type_name, const std::string& path_key, const std::string& value) const {
        return metadata.at(type_name).parsePropertyValue(path_key, value);
    }

    void setProperties(StringRef type_name, std::shared_ptr<void> instance, ElementProperties properties) {
        metadata.at(type_name).setProperties(instance, properties);
    }

    std::shared_ptr<void> createObject(StringRef type_name,  std::optional<GenericRef> rfl_generic) {
        return metadata.at(type_name).createObject(rfl_generic);
    }

    std::shared_ptr<void> createStorageObject(StringRef type_name, EntityRef entity, std::optional<GenericRef> rfl_generic) {
        return metadata.at(type_name).createStorageObject(entity, rfl_generic);
    }

    template<typename T>
    std::shared_ptr<void> createStorageObject(EntityRef entity, std::optional<GenericRef> rfl_generic) {
        StringRef type_name = getTypeName<T>();
        auto obj =  metadata.at(type_name).createStorageObject(entity, rfl_generic);
        return std::static_pointer_cast<T>(obj);
    }

    void registerStorageObjectCreator(StringRef type_name, StorageObjectCreator&& creator) {
        metadata.at(type_name).registerStorageObjectCreator(creator); 
    }

private:
    std::unordered_map<StringRef, PropertyMeta> metadata;
};
#endif
