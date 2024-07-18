#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <functional>
#include <variant>
#include <any>
#include <refl.hpp>
#include "Property.h"
#include "Port.h"

using ValueParser = ValueType(*)(const std::string&, const std::string&);
using PropertiesSetter = void(*)(const std::shared_ptr<void>, ElementProperties&);

class PropertyMeta {
public:

    template <typename T>
    static PropertyMeta createMetadata()
    {
        constexpr auto type_name = refl::reflect<T>().name;
        PropertyMeta md;
        md.name_ = type_name.c_str();
        md.value_parser_ = &PropertyMeta::ParsePropertyValue<T>;
        md.properties_setter_ = &PropertyMeta::SetProperties<T>;
        return md;
    }

    ValueType parsePropertyValue(const std::string& path_key, const std::string& value) const {
        return value_parser_(path_key, value);
    }

    void setProperties(std::shared_ptr<void> instance, ElementProperties properties) {
        return properties_setter_(instance, properties);
    }

    std::string_view name() const {
        return name_;
    }

private:
    std::string name_;
    ValueParser value_parser_;
    PropertiesSetter properties_setter_;

    template <typename T>
    static ValueType& ParsePropertyValue(std::string& path_key, std::string& value) {
        for_each(refl::reflect<T>().members, [&](auto member) {
            if (path_key == "*" || member.name.str() == path_key) {
                using member_type = typename decltype(member)::value_type;
                const auto& prop = refl::descriptor::get_attribute<Property>(member);
                return ValueType(prop.template parse<member_type>(value));
                //return TypeManager::parseValue<member_type>(prop, value);
            }
        });
    }

    template <typename T>
    static void SetProperties(std::shared_ptr<void> instance, ElementProperties properties) {
        auto instanceT = std::static_pointer_cast<T>(instance);
        for_each(refl::reflect<T>().members, [&](auto member) {
            if constexpr (refl::descriptor::has_attribute<Property>(member)) {
                auto&& prop = refl::descriptor::get_attribute<Property>(member);
                if (auto propIter = properties.find(member.name.str()); propIter != properties.end()) {
                    using member_type = typename decltype(member)::value_type;
                    member(*instanceT) = std::visit([&](auto&& arg) -> member_type {
                        if constexpr (std::is_same_v<member_type, decltype(arg)>) {
                            return arg;
                        } else if constexpr (std::is_same_v<std::any, decltype(arg)>) {
                            return std::any_cast<member_type>(arg);
                        } else {
                            throw std::runtime_error("Unsupported type conversion");
                        }
                    }, propIter->second);
                }
            }
        });
    }

    //template<typename member_type, typename Parser = std::nullptr_t>
    //static ValueType parseValue(const Property<Parser>& prop, const std::string& valueStr) {
        ////if constexpr (std::is_same_v<Parser, std::nullptr_t>) {
            ////return default_parser<member_type>()(valueStr);
        ////} else {
            //return ValueType(prop.template parse<member_type>(valueStr));
        ////}
    //}

    ////template <typename T>
    //static GetPropertyValue(std::shared_ptr<void> instance, const ElementProperties& props)
    //{
        //for_each(refl::reflect<T>().members, [&](auto member) {
            //if constexpr (refl::descriptor::has_attribute<Property>(member)) {
                //auto&& prop = refl::descriptor::get_attribute<Property>(member);
                //if (auto propIter = props.find(member.name.str()); propIter != props.end()) {
                    //member(instance.get()) = std::visit([&](auto&& arg) -> member_type {
                        //if constexpr (std::is_same_v<member_type, decltype(arg)>) {
                            //return arg;
                        //} else if constexpr (std::is_same_v<std::any, decltype(arg)>) {
                            //return std::any_cast<member_type>(arg);
                        //} else {
                            //throw std::runtime_error("Unsupported type conversion");
                        //}
                    //}, it->second);
                //}
            //}
        //});


        /**
         * for_each loop above essentially gets compiled to multiples of the following (pseudo-code):
         * if (auto propIter = props.find("MemberA"); propIter != props.end()) {
         *     instance.MemberA = prop.parser(propIter->second);
         * }
         */

        //return instance;
    //}
};

class TypeInfo
{
public:

    // instances can be obtained only through calls to Get()
    template <typename T>
    static const TypeInfo& Get()
    {
        // here we create the singleton instance for this particular type
        static const TypeInfo ti(refl::reflect<T>());
        return ti;
    }

    const std::string& Name() const
    {
        return name_;
    }

private:

    // were only storing the name for demonstration purposes,
    // but this can be extended to hold other properties as well
    std::string name_;

    // given a type_descriptor, we construct a TypeInfo
    // with all the metadata we care about (currently only name)
    template <typename T, typename... Fields>
    TypeInfo(refl::type_descriptor<T> td)
        : name_(td.name)
    {
    }

};

class TypeManager {
public:
    static TypeManager& instance() {
        static TypeManager instance;
        return instance;
    }

    TypeManager() = default;

    template <typename T>
    const std::string& getTypeName() {
        auto& ti = TypeInfo::Get<::refl::trait::remove_qualifiers_t<T>>();
        return ti.Name();
    }

    template <typename T>
    const std::string& registerType() {
        auto& type_name = getTypeName<T>();
        auto it = metadata.find(type_name);
        if (it != metadata.end()) {
            metadata[type_name] = PropertyMeta::createMetadata<T>();
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

    //std::shared_ptr<void> setupElement(std::shared_ptr<void> instance, const std::string& type_name, const ElementProperties& props) {
        //auto it = metadata.find(type_name);
        //if (it != metadata.end()) {
            //it->setupELement(instance, props);
            //return instance;
        //}
        //throw std::runtime_error("Type not registered: " + type_name);
    //}

    //void initialize(std::shared_ptr<void> instance, const std::string& typeName, const ElementProperties& props) {
        //const auto& type = *typeDescriptors_.at(typeName);
        //refl::runtime::for_each(type.members, [&](auto member) {
            //if constexpr (refl::descriptor::has_attribute<Property<>>(member)) {
                //using member_type = typename decltype(member)::value_type;
                //const auto& prop = refl::descriptor::get_attribute<Property<>>(member);
                //if (auto it = props.find(member.name.str()); it != props.end()) {
                    //member(instance.get()) = std::visit([&](auto&& arg) -> member_type {
                        //if constexpr (std::is_same_v<member_type, decltype(arg)>) {
                            //return arg;
                        //} else if constexpr (std::is_same_v<std::any, decltype(arg)>) {
                            //return std::any_cast<member_type>(arg);
                        //} else {
                            //throw std::runtime_error("Unsupported type conversion");
                        //}
                    //}, it->second);
                //}
            //}
        //});
    //}


    template<typename member_type, typename Parser = std::nullptr_t>
    static ValueType parseValue(const Property<Parser>& prop, const std::string& valueStr) {
        //if constexpr (std::is_same_v<Parser, std::nullptr_t>) {
            //return default_parser<member_type>()(valueStr);
        //} else {
            return ValueType(prop.template parse<member_type>(valueStr));
        //}
    }

    const std::unordered_map<std::string, PropertyMeta>& getTypePropertyMeta() const {
        return metadata;
    }
    //}


    ValueType parsePropertyValue(const std::string& type_name, const std::string& path_key, const std::string& value) const {
        return metadata.at(type_name).parsePropertyValue(path_key, value);
        //auto it = metadata.find(type_name);
        //if (it != metadata.end()) {
            //return (it->second).getPropertyValue(path_key, value);
        //}
        //throw std::runtime_error("Type is not registered!");
    }

    void setProperties(const std::string& type_name, std::shared_ptr<void> instance, ElementProperties properties) {
        metadata.at(type_name).setProperties(instance, properties);
    }

private:
    std::unordered_map<std::string, PropertyMeta> metadata;


    //std::unordered_map<std::string, ConstructorFunction> constructors_;
    //std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> factories_;
    // std::unordered_map<std::string, const refl::descriptor::type_descriptor*> typeDescriptors_;
    //std::unordered_map<std::string, std::any> typeDescriptors_;
};
