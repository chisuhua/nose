#pragma once
#include "IPort.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include "Registry.h"

class Wire {
public:
    std::vector<std::shared_ptr<IPort>> connect;

    virtual ~Wire() = default; // 添加虚拟析构函数

    virtual void Bind() {
        if (connect.size() < 2) {
            throw std::runtime_error("Insufficient connected ports for binding");
        }

        auto masterPort = connect[0];
        auto slavePort = connect[1];

        if (masterPort->role != Role::Master || slavePort->role != Role::Slave) {
            throw std::runtime_error("Invalid port roles for binding");
        }

        masterPort->bind(slavePort);
        slavePort->bind(masterPort);
    }
};

REFL_AUTO(
    type(Wire),
    field(connect)
    );

