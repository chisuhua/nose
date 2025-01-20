#ifndef WIRE_H
#define WIRE_H

#include "Port.h"
#include "EntityIntern.h"
#include <stdexcept>
#include <rfl.hpp>

class Wire;

struct WireParam {
    EntityHashType entity_hash;
    std::shared_ptr<PortParam> master_;
    std::shared_ptr<PortParam> slave_;
    PortRole role_;
    uint64_t test_only;
};

class Wire {
public:
    using GenericType = std::shared_ptr<WireParam>;
    GenericType generic_;

    static std::shared_ptr<Wire> GetInstance(GenericType generic) {
        auto entity = EntityRef::getEntityByHash(generic->entity_hash);
        return entity.getObject<Wire>();
    }

    explicit Wire(GenericType generic);
    Wire() = delete;

    virtual void bind() {
        if (!generic_->master_ or !generic_->slave_) {
            throw std::runtime_error("Insufficient connected ports for binding");
        }

        auto master_port = Port::GetInstance(generic_->master_);
        auto slave_port = Port::GetInstance(generic_->slave_);

        if (master_port->getRole() != PortRole::Master || slave_port->getRole() != PortRole::Slave) {
            throw std::runtime_error("Invalid port roles for binding");
        }

        master_port->bind(slave_port);
        slave_port->bind(master_port);
    }

    void setMasterPort(std::shared_ptr<Port> master) {
        generic_->master_ = master->generic_;
    }

    void setSlavePort(std::shared_ptr<Port> slave) {
        generic_->master_ = slave->generic_;
    }

    uint64_t test_only;
    std::vector<Port> connect_;
};

// ValueType ParseConnection(const std::string& valueStr) {
//     // TODO
//     return ValueType(std::any(valueStr));
// };

REFL_AUTO(
    type(Wire),
    field(generic_),
    field(test_only),
    field(connect_)
);

#endif // W

