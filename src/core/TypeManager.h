#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <functional>
#include <variant>
#include <any>
#include <Port.h>
#include <ValueType.h>

class TypeManager {
public:
    //using ValueType = std::variant<int, float, std::string, bool, double, uint64_t, std::any>;
    using ConstructorFunction = std::function<std::shared_ptr<void>(const std::map<std::string, std::any>&)>;

    static TypeManager& instance() {
        static TypeManager instance;
        return instance;
    }

    template<typename member_type, typename Parser = std::nullptr_t>
    static ValueType parseValue(const IoProperty<Parser>& prop, const std::string& valueStr) {
        return ValueType(prop.template parse<member_type>(valueStr));
        //if constexpr (std::is_same_v<Parser, std::nullptr_t>) {
            //return default_parser<member_type>()(valueStr);
        //} else {
        //}
    }

    const std::unordered_map<std::string, ConstructorFunction>& getTypeConstructors() const {
        return constructors_;
    }

    template<typename T>
    void registerType(const std::string& typeName) {
        constructors_[typeName] = [](const std::map<std::string, std::any>& properties) {
            return std::make_shared<T>();
        };
    }

private:
    TypeManager() {}

    std::unordered_map<std::string, ConstructorFunction> constructors_;
};

