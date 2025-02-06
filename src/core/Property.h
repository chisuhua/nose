#ifndef PROPERTY_H
#define PROPERTY_H
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <variant>
#include <any>
#include <refl.hpp>
#include <rfl/json.hpp>
#include <rfl.hpp>
#include <sstream>
#include <map>
#include <vector>

// 定义 ValueType 作为节点的属性值类型
using ValueType = std::variant<nullptr_t, int, float, std::string, bool, double, uint64_t, rfl::Generic, std::any>;
using ElementProperties = std::unordered_map<std::string, ValueType>;
using GenericRef = std::reference_wrapper<const rfl::Generic>;
using GenericPtr = std::shared_ptr<rfl::Generic>;
using RflGeneric = rfl::Generic;

template <typename T, typename = void>
struct is_reflectable : std::false_type {};

template <typename T>
struct is_reflectable<
    T,
    std::void_t<
        std::conditional_t<
            refl::trait::is_reflectable<T>::value ||
            std::is_convertible_v<T, rfl::Generic::VariantType>,
            void,
            std::false_type
        >
    >
> : std::true_type {};

#if 0
template <typename T>
struct always_false : std::false_type {};

// 辅助结构体检测是否为容器类型
template <typename T, typename _ = void>
struct is_container : std::false_type {};

template <typename... Ts>
struct is_container_helper {};

template <typename T>
struct is_container<
    T,
    std::conditional_t<
        false,
        is_container_helper<
            typename T::value_type,
            typename T::size_type,
            typename T::allocator_type,
            typename T::iterator,
            typename T::const_iterator,
            decltype(std::declval<T>().size()),
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end())>,
        void>> : std::true_type {};
#endif

// 默认解析器
namespace parser {
    static std::string trim(const std::string& str) {
        const char* whitespace = " \t\n\r\f\v";
        size_t start = str.find_first_not_of(whitespace);
        size_t end = str.find_last_not_of(whitespace);

        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    //static std::vector<std::string> splitInitializer(const std::string& str) {
        //std::vector<std::string> elements;
        //std::istringstream stream(str.substr(1, str.size() - 2)); // 去掉最外层大括号
        //std::string elem;
        //while (std::getline(stream, elem, ',')) {
            //elements.push_back(trim(elem));
        //}
        //return elements;
    //}

    template <typename T>
    ValueType default_parser(const std::string& valueStr);

#if 0
    template <typename Container>
    Container parseContainer(const std::string& valueStr) {
        Container container;
        if constexpr (is_container<Container>::value && std::is_same_v<Container, std::vector<typename Container::value_type>>) {
            std::vector<std::string> elements = splitInitializer(valueStr);
            for (const auto& element : elements) {
                container.emplace_back(default_parser<typename Container::value_type>()(element));
            }
        } else if constexpr (is_container<Container>::value && std::is_same_v<Container, std::map<typename Container::key_type, typename Container::mapped_type>>) {
            std::vector<std::string> elements = splitInitializer(valueStr);
            for (const auto& element : elements) {
                auto colonPos = element.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = trim(element.substr(0, colonPos));
                    std::string val = trim(element.substr(colonPos + 1));
                    container[default_parser<typename Container::key_type>()(key)] = default_parser<typename Container::mapped_type>()(val);
                }
            }
        }
        return container;
    }
#endif

    template <typename T>
    ValueType default_parser(const std::string& valueStr) {
        if constexpr (std::is_same_v<T, int>) {
            return ValueType(std::stoi(valueStr));
        } else if constexpr (std::is_same_v<T, float>) {
            return std::stof(valueStr);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return valueStr;
        } else if constexpr (std::is_same_v<T, bool>) {
            return valueStr == "true" || valueStr == "1";
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(valueStr);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return std::stoul(valueStr);
        } else /*if constexpr (is_container<T>::value)*/ {
            //return ValueType(std::any(parseContainer<T>(valueStr)));
            return ValueType(rfl::json::read<rfl::Generic>(valueStr).value());
            /*
        } else {
            static_assert(always_false<T>::value, "Unsupported type for default_parser, please use customer paser function");
            */
        }
    }
}

enum class PropertyType
{
    Default = 0b0,
    Required = 0b1,
    Content = 0b10,
    RequiredContent = Required | Content
};

//template <typename T>
//using ParseFunction = std::function<T(const std::string&)>;
//using ParserFuncType = std::function<ValueType(const std::string&)>;
typedef ValueType (*ValueParser)(const std::string&);


template <typename Parser = std::nullptr_t>
struct Property : refl::attr::usage::field {
    //using Parser = ParseFunction<T>;
    //using Parser = decltype(parser::default_parser
    const Parser parser;
    const PropertyType type;
    constexpr Property(const Parser& parser = nullptr)
        : parser(parser) 
        , type(PropertyType::Default) 
    {}

    constexpr Property(PropertyType type, const Parser& parser = nullptr)
        : parser(parser) 
        , type(type) 
    {}

    template <typename member_type>
    ValueType parse(const std::string& value) const {
        if constexpr (std::is_same_v<Parser, std::nullptr_t>) {
            return parser::default_parser<member_type>(value);
        } else {
            return parser(value);
        }
    }
};
#endif
