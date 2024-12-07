#pragma once
#include "IPort.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include "Registry.h"

class Wire {
public:
    std::vector<std::shared_ptr<IPort>> connect_;
    uint64_t test_only;

    virtual ~Wire() = default; // 添加虚拟析构函数

    virtual void Bind() {
        if (connect_.size() < 2) {
            throw std::runtime_error("Insufficient connected ports for binding");
        }

        auto masterPort = connect_[0];
        auto slavePort = connect_[1];

        if (masterPort->getRole() != Role::Master || slavePort->getRole() != Role::Slave) {
            throw std::runtime_error("Invalid port roles for binding");
        }

        masterPort->bind(slavePort);
        slavePort->bind(masterPort);
    }
};

//ValueType ParseConnection(const std::string& valueStr) {
    //// TODO
    //return ValueType(std::any(valueStr));
//};

REFL_AUTO(
    type(Wire),
    field(test_only),
    field(connect_)
);

