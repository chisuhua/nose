#pragma once
#include "IPort.h"
#include <tuple>
#include <string>
#include <type_traits>

// 合并多个结构体到一个结构体
template<typename... Ts>
struct MergeTypes : Ts... {
    MergeTypes() = default;
};

// 确保类型安全
template<typename... Ts>
MergeTypes(Ts...) -> MergeTypes<Ts...>;

// IOType 模板类
template<typename... Ts>
struct IOType {
    MergeTypes<Ts...> io_;

    IOType() {
        (initialize<Ts>(), ...);
    }

private:
    template<typename T>
    void initialize() {
        using namespace refl;
        const auto type_info = reflect<T>();
        for_each(type_info.members, [this](auto member) {
            const auto& io_prop = refl::descriptor::get_attribute<IoProperty<>>(member);
            set_attribute(member.name.c_str(), member(this->io_), io_prop.role);
        });
    }

    template<typename MemberType>
    void set_attribute(const std::string& name, MemberType& member_ref, Role role) {
        using namespace refl;
        const auto io_type_info = reflect<decltype(io_)>();

        for_each(io_type_info.members, [this, &name, &member_ref, &role](auto member) {
                // FIXME on refl set_attribute
            //if (member.name.c_str() == name) {
                //refl::descriptor::set_attribute<decltype(io_)>(member.member_pointer, member_ref, IoProperty<>(role));
            //}
        });
    }
};


template<typename IOType>
class Port : public IPort {
public:
    Port() = default;

    const IOType& getIO() const {
        return io_;
    }

    std::any getIO() override {
        return io_;
    }

    void bind(std::shared_ptr<IPort> other) override {
        peer = other;
    }

    std::shared_ptr<IPort> clone() const override {
        auto clonedPort = std::make_shared<Port<IOType>>();
        clonedPort->io_ = this->io_;
        clonedPort->role = this->role;
        return clonedPort;
    }

private:
    IOType io_;
    std::function<void()> observer_;
};

