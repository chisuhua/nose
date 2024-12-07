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

class Trans;

enum class Role {
    Master,
    Slave,
};

class IPort {
    using SelfPtr = std::shared_ptr<IPort>;
public:
    IPort() : peer_(nullptr) {}

    IPort(const IPort& _other): role(_other.role)
    {}

    ~IPort() = default;

    Role getRole() { return role; }
    void setRole(Role role_) { role = role_; }

    void bind(SelfPtr other) {
            peer_ = other;
    }

    IPort& operator << (Trans* t) {
        auto& peer_data = peer_->trans_;
        peer_data.push_back(t);
        //this->trans_.push(t);
        return *this;
    }

    IPort& operator >> (Trans* t) {
        auto& peer_data = peer_->trans_;
        t = this->trans_.back();
        this->trans_.pop_back();
        return *this;
    }

    std::shared_ptr<IPort> clone() const {
        auto clonedPort = std::make_shared<IPort>();
        clonedPort->peer_ = this->peer_;
        clonedPort->role = this->role;
        return clonedPort;
    }

    SelfPtr getPeer() { return peer_; }
private:
    SelfPtr peer_;
    std::vector<Trans*> trans_;
    Role role;
    
};

REFL_AUTO(
    type(IPort),
    field(role)
    )

