#pragma once
#include <stdexcept>
#include <iostream>
#include "Wire.h"
#include "Registry.h"

class Channel : public Wire {
public:
    // 虚拟析构函数
    virtual ~Channel() override {
        delete masterPort_;
        delete slavePort_;
    }

    Port* masterPort_ = nullptr;
    Port* slavePort_ = nullptr;

    virtual void Bind() override {
        if (connect_.size() < 2) {
            throw std::runtime_error("Insufficient connected ports for binding");
        }

        // 克隆连接的端口
        masterPort_ = connect_[0]->clone().release();
        slavePort_ = connect_[1]->clone().release();

        if (masterPort_->getRole() != PortRole::Master || slavePort_->getRole() != PortRole::Slave) {
            throw std::runtime_error("Invalid port roles for binding");
        }

        // 绑定端口
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

// ValueType ParsePort(const std::string& valueStr) {
//     // TODO
//     return ValueType(std::any(valueStr));
// };

REFL_AUTO(
    type(Channel, bases<Wire>),
    field(masterPort_),
    field(slavePort_)
);

