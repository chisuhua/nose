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
        if (connect.size() < 2) {
            throw std::runtime_error("Insufficient connected ports for binding");
        }

        masterPort_ = connect[0]->clone();
        slavePort_ = connect[1]->clone();

        if (masterPort_->role != Role::Master || slavePort_->role != Role::Slave) {
            throw std::runtime_error("Invalid port roles for binding");
        }

        masterPort_->bind(connect[1]);
        slavePort_->bind(connect[0]);

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

REFL_AUTO(
        type(Channel, bases<Wire>),
        field(masterPort_),
        field(slavePort_)
        )

