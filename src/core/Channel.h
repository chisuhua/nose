#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include "Wire.h"
#include "Registry.h"


class Channel : public Wire {
public:
    using GenericType = std::shared_ptr<WireData>;
    //GenericType generic_;

    explicit Channel(GenericType generic) 
        : Wire(generic) {}

    // 虚拟析构函数
    //virtual ~Channel() override {
        //delete masterPort_;
        //delete slavePort_;
    //}

    virtual void bind() override {
        Wire::bind();
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
    type(Channel, bases<Wire>)
    //field(generic_)
);
REGISTER_OBJECT(Channel)
#endif
