#ifndef WIRE_H
#define WIRE_H

#include "Port.h"
#include "Path.h"
#include <stdexcept>
#include <rfl.hpp>
#include "Registry.h"

class Wire;

struct WireData {
    EntityHashType entity_hash;
    ObjPtr<Port> master_;
    ObjPtr<Port> slave_;
    PortRole role_;
    uint64_t test_only;
};

struct WireConnect {
  std::string master_name;
  std::string slave_name;
};

class Wire {
public:
    using GenericType = std::shared_ptr<WireData>;
    GenericType generic_;

    explicit Wire(GenericType generic) 
        : generic_(generic) {}

    virtual void bind() {
        if (!generic_->master_.ptr()) {
            generic_->master_ = Path::make(connect_.master_name).make_object<Port>();
            generic_->master_->setRole(PortRole::Master);
        }
        if (!generic_->slave_.ptr()) {
            generic_->slave_ = Path::make(connect_.slave_name).make_object<Port>();
            generic_->slave_->setRole(PortRole::Slave);
        }
        auto master_port = generic_->master_;
        auto slave_port = generic_->slave_;
        if (!master_port.ptr() or !slave_port.ptr()) {
            throw std::runtime_error("Insufficient connected ports for binding");
        }

        if (master_port->getRole() != PortRole::Master || slave_port->getRole() != PortRole::Slave) {
            throw std::runtime_error("Invalid port roles for binding");
        }

        master_port->bind(slave_port);
        slave_port->bind(master_port);
    }

    void setMasterPort(ObjPtr<Port> master) {
        generic_->master_ = master;
        generic_->master_->setRole(PortRole::Master);
    }

    void setSlavePort(ObjPtr<Port> slave) {
        generic_->slave_ = slave;
        generic_->slave_->setRole(PortRole::Slave);
    }

    ObjPtr<Port> getMasterPort() {
        return generic_->master_;
    }

    ObjPtr<Port> getSlavePort() {
        return generic_->slave_;
    }

    uint64_t test_only;
    WireConnect connect_;
};

// ValueType ParseConnection(const std::string& valueStr) {
//     // TODO
//     return ValueType(std::any(valueStr));
// };

REFL_AUTO(
    type(Wire),
    field(test_only),
    field(connect_)
);
REGISTER_OBJECT(Wire)

#endif // W

