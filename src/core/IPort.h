#pragma once
#include <memory>
#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <refl.hpp>
#include <ValueType.h>

enum class Role {
    Master,
    Slave,
};

Role parse_role(std::string_view str) {
    if (str == "master") return Role::Master;
    if (str == "slave") return Role::Slave;
    throw std::runtime_error("Cannot parse " + std::string(str) + " as role");
}



template <typename Parser = decltype(parse_role)>
struct IoProperty : refl::attr::usage::field {
    const Role role;
    const Parser parser;
    constexpr IoProperty(Role role) : role(role), parser(parse_role) {}

    template<typename T>
    T parse(const std::string& value) const {
        if constexpr (std::is_same_v<Parser, std::nullptr_t>) {
            return default_parser<T>()(value);
        } else {
            return parser(value);
        }
    }
};

class IPort {
public:
    virtual ~IPort() = default;
    Role role;
    std::shared_ptr<IPort> peer;

    virtual std::any getIO() = 0;
    virtual void bind(std::shared_ptr<IPort> other) = 0;
    virtual std::shared_ptr<IPort> clone() const = 0;
};

