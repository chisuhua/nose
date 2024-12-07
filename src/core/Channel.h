#pragma once
#include <stdexcept>
#include <iostream>
#include "Wire.h"
#include "Registry.h"

class Channel : public Wire {
public:
    virtual ~Channel() = default; // 添加虚拟析构函数
                               //
    std::shared_ptr<IPort> masterPort_;
    std::shared_ptr<IPort> slavePort_;

    virtual void Bind() override {
        if (connect_.size() < 2) {
            throw std::runtime_error("Insufficient connected ports for binding");
        }

        masterPort_ = connect_[0]->clone();
        slavePort_ = connect_[1]->clone();

        if (masterPort_->getRole() != Role::Master || slavePort_->getRole() != Role::Slave) {
            throw std::runtime_error("Invalid port roles for binding");
        }

        masterPort_->bind(connect_[1]);
        slavePort_->bind(connect_[0]);

        masterPort_->bind(slavePort_);
        slavePort_->bind(masterPort_);
    }

    virtual void tick() {
        std::cout << "Channel clock update" << std::endl;
    }

    virtual void portNotified(const std::string& portName) {
        std::cout << "Channel notified for port: " << portName << std::endl;
    }
};

//ValueType ParsePort(const std::string& valueStr) {
    ////TODO
    //return ValueType(std::any(valueStr));
//};

REFL_AUTO(
        type(Channel, bases<Wire>),
        field(masterPort_),
        field(slavePort_)
        );

