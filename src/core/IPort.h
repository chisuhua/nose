#pragma once
#include <memory>
#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <Property.h>
#include <functional>
#include <string_view>
#include <refl.hpp>

enum class Role {
    Master,
    Slave,
};

Role parse_role(std::string_view str) {
    if (str == "master") return Role::Master;
    if (str == "slave") return Role::Slave;
    throw std::runtime_error("Cannot parse " + std::string(str) + " as role");
}

//template <typename T>
//using ParseFunction = std::function<T(const std::string&)>;

//template <typename Parser = ParseFunction<Role>>
//struct IoProperty : refl::attr::usage::field {
    //const Role role;
    //const Parser parser;

    //constexpr IoProperty(Role role, Parser parser = ParseFunction<Role>(parse_role))
        //: role(role), parser(parser) {}

    //template<typename T>
    //T parse(const std::string& value) const {
        //return parser(value);
    //}
//};

class IPort {
public:
    virtual ~IPort() = default;
    Role role_;
    Role getPortRole() { return role_; }

    //virtual std::any getIO() = 0;
    virtual void bind(std::shared_ptr<IPort> other) = 0;
    virtual std::shared_ptr<IPort> clone() const = 0;
};

ValueType ParseRole(const std::string& valueStr) {
    return ValueType(std::any(parse_role(valueStr)));
}

REFL_AUTO(
    type(IPort),
    field(role_, Property<ValueParser>(&ParseRole))
    )

