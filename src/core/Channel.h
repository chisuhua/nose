#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include "Wire.h"
#include "Registry.h"

struct ChannelData {
    EntityHashType entity_hash;
    ObjPtr<Port> master_;
    ObjPtr<Port> slave_;
    PortRole role_;
};

struct ChannelConnect {
  std::string master_name;
  std::string slave_name;
};

class Channel {
public:
    using GenericType = std::shared_ptr<ChannelData>;
    GenericType generic_;

    explicit Channel(GenericType generic) 
        : generic_(generic) {}

    // 虚拟析构函数
    //virtual ~Channel() override {
        //delete masterPort_;
        //delete slavePort_;
    //}

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

    ChannelConnect connect_;


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
    type(Channel),
    field(connect_)
);
REGISTER_OBJECT(Channel)
#endif
