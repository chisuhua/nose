#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <variant>
#include <any>
#include <refl.hpp>
#include <sstream>
#include <map>
#include <vector>

// 定义 ValueType 作为节点的属性值类型
using ValueType = std::variant<int, float, std::string, bool, double, uint64_t, std::any>;
using ElementProperties = std::unordered_map<std::string, ValueType>;

// 辅助模板判断类型
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

// 默认解析器
template <typename T>
struct default_parser {
    T operator()(const std::string& valueStr) const {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(valueStr);
        } else if constexpr (std::is_same_v<T, float>) {
            return std::stof(valueStr);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return valueStr;
        } else if constexpr (std::is_same_v<T, bool>) {
            return valueStr == "true" || valueStr == "1";
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(valueStr);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return std::stoull(valueStr);
        } else if constexpr (is_container<T>::value) {
            return ValueType(std::any(parseContainer<T>(valueStr)));
        } else {
            static_assert(always_false<T>::value, "Unsupported type for default_parser");
        }
    }

    template <typename Container>
    Container parseContainer(const std::string& valueStr) const {
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

    static std::vector<std::string> splitInitializer(const std::string& str) {
        std::vector<std::string> elements;
        std::istringstream stream(str.substr(1, str.size() - 2)); // 去掉最外层大括号
        std::string elem;
        while (std::getline(stream, elem, ',')) {
            elements.push_back(trim(elem));
        }
        return elements;
    }

    static std::string trim(const std::string& str) {
        const char* whitespace = " \t\n\r\f\v";
        size_t start = str.find_first_not_of(whitespace);
        size_t end = str.find_last_not_of(whitespace);

        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }
};

enum class PropertyType
{
    Default = 0b0,
    Required = 0b1,
    Content = 0b10,
    RequiredContent = Required | Content
};

template <typename Parser>
struct Property : refl::attr::usage::field {
    const Parser parser;
    const PropertyType type;
    constexpr Property(const Parser& parser) 
        : parser(parser) 
        , type(PropertyType::Default) 
    {}

    constexpr Property(PropertyType type, const Parser& parser) 
        : parser(parser) 
        , type(type) 
    {}

    template<typename T>
    T parse(const std::string& value) const {
        if constexpr (std::is_same_v<Parser, std::nullptr_t>) {
            return default_parser<T>()(value);
        } else {
            return parser(value);
        }
    }
};

