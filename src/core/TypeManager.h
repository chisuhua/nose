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
#include <typeindex>
#include <refl.hpp>
#include "Path.h"
#include "StringIntern.h"
#include "Property.h"
#include "TypeInfo.h"

using KeyValueParser = ValueType(*)(const std::string&, const std::string&);
using PropertiesSetter = void(*)(const std::shared_ptr<void>, ElementProperties&);

using ObjectCreator = std::function<std::shared_ptr<void>(GenericRef)>;
template <typename T>
using StorageObjectCreator_t = std::function<std::shared_ptr<void>(Path, T)>;
using StorageObjectCreator = std::function<std::shared_ptr<void>(Path, std::any)>;

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
        } else {
            auto type_name = TypeInfo::Get<T>().Name();
            md.name_ = type_name;
            md.value_parser_ = &PropertyMeta::ParsePropertyValue<T>;
            md.properties_setter_ = &PropertyMeta::SetProperties<T>;
            md.object_creator_ = &PropertyMeta::CreateObject<T>;
        }

        return md;
    }

    ValueType parsePropertyValue(const std::string& path_key, const std::string& value) const {
        return value_parser_(path_key, value);
    }

    void setProperties(std::shared_ptr<void> instance, ElementProperties& properties) {
        return properties_setter_(instance, properties);
    }

    std::shared_ptr<void> createObject(GenericRef rfl_generic) {
        return object_creator_(rfl_generic);
    }

    template <typename ArgType>
    std::shared_ptr<void> createStorageObject(const Path& entity, ArgType&& args) {
        std::cout << name_->str() << " run creator with typeid " << typeid(ArgType).hash_code() << "for type " << TypeInfo::getTypeName<ArgType>()->str() << std::endl;
        auto it = storage_object_creators_.find(typeid(ArgType));
        if (it == storage_object_creators_.end()) {
            throw std::runtime_error("creator not registered for this argument types\n");
        }
        return it->second(entity, args);
    }

    template <typename ArgType>
    void registerStorageObjectCreator(std::function<std::shared_ptr<void>(Path, ArgType&& arg)> creator) {
        std::cout << name_->str() << " register creator with typeid " << typeid(ArgType).hash_code() << "for type " << TypeInfo::getTypeName<ArgType>()->str() << std::endl;
        storage_object_creators_[typeid(ArgType)] = [creator](const Path& entity, std::any arg) -> std::shared_ptr<void> {
            return creator(entity, std::any_cast<ArgType>(arg));
        };
    }

    StringRef name() const {
        return name_;
    }

private:
    StringRef name_;
    KeyValueParser value_parser_;
    PropertiesSetter properties_setter_;
    ObjectCreator object_creator_;
    std::unordered_map<std::type_index, StorageObjectCreator> storage_object_creators_;

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
                                if constexpr(has_generic_v<member_type>) {
                                    //using GenericType = typename member_type::GenericType;
                                    using GenericType = ExtractedGenericType<member_type>;
                                    return std::make_shared(rfl::from_generic<GenericType>(arg).value());
                                } else {
                                    return rfl::from_generic<member_type>(arg).value();
                                }
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
    static std::shared_ptr<void> CreateObject(GenericRef rfl_generic) {
        if constexpr(has_generic_v<T>) {
            using GenericType = ExtractedGenericType<T>;
            //if (rfl_generic) {
                GenericType obj = rfl::from_generic<GenericType>(rfl_generic).value();
                return std::static_pointer_cast<void>(std::make_shared<T>(std::make_shared<GenericType>(std::move(obj))));
            //} else {
                //GenericType obj;
                //return std::static_pointer_cast<void>(std::make_shared<T>(std::make_shared<GenericType>(std::move(obj))));
            //}
        } else {
            T obj = rfl::from_generic<T>(rfl_generic).value();
            return std::static_pointer_cast<void>(std::make_shared<T>(obj));
        }
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
                using GenericType = typename T::GenericType::element_type;
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

    std::shared_ptr<void> createObject(StringRef type_name, GenericRef rfl_generic) {
        return metadata.at(type_name).createObject(rfl_generic);
    }

    //template <typename... Args>
    //std::shared_ptr<void> createStorageObject(StringRef type_name, Path entity, Args&&... args) {
        //return metadata.at(type_name).createStorageObject(entity, std::forward<Args>(args)...);
    //}

    //template <typename T, typename... Args>
    //std::shared_ptr<T> createStorageObject(Path entity, Args&&... args) {
        //StringRef type_name = getTypeName<T>();
        //auto obj = metadata.at(type_name).createStorageObject(entity, std::forward<Args>(args)...);
        //return std::static_pointer_cast<T>(obj);
    //}

    //template <typename... Args>
    //void registerStorageObjectCreator(StringRef type_name, std::function<std::shared_ptr<void>(Path, Args...)> creator) {
        //metadata.at(type_name).registerStorageObjectCreator<Args...>(creator);
    //}

    template <typename ArgType>
    std::shared_ptr<void> createStorageObject(StringRef type_name, const Path& entity, ArgType&& args) {
        return metadata.at(type_name).createStorageObject(entity, std::forward<ArgType>(args));
    }

    template <typename T, typename ArgType>
    std::shared_ptr<T> createStorageObject(const Path& entity, ArgType&& args) {
        StringRef type_name = getTypeName<T>();
        auto obj = metadata.at(type_name).createStorageObject(entity, std::forward<ArgType>(args));
        return std::static_pointer_cast<T>(obj);
    }

    template <typename ArgType>
    void registerStorageObjectCreator(StringRef type_name, std::function<std::shared_ptr<void>(Path, ArgType arg)> creator) {
        metadata.at(type_name).registerStorageObjectCreator<ArgType>(creator);
    }

private:
    std::unordered_map<StringRef, PropertyMeta> metadata;
};

#endif // TYPEMANAGER_H

