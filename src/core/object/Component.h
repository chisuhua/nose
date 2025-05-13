#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include "Port.h"

struct ComponentData {
    // 端口映射
    std::map<std::string,ObjPtr<Port>> ports_;
    // 端口更新标志映射
    std::map<std::string, bool> portsUpdated_;

    uint64_t test_only;
};

class Component {
public:
    using GenericType = std::shared_ptr<ComponentData>;
    GenericType generic_;

    explicit Component(GenericType generic) 
        : generic_(generic) {}


    virtual ~Component() = default;
    virtual void build() {};

    // 添加端口
    void addPort(const std::string& name, ObjPtr<Port> port) {
        std::cout << "  add component port: port_name=" << name << " port_obj " << port.ptr() << "\n";
        generic_->ports_[name] = port;
    }

    // 获取端口
    ObjPtr<Port> getPort(const std::string& name) {
        auto it = generic_->ports_.find(name);
        if (it != generic_->ports_.end()) {
            return it->second;
        }
        //return ObjPtr<Port>::make(nullptr);
    }

    // 端口通知
    virtual void updatePort(const std::string& portName) {
        generic_->portsUpdated_[portName] = true;
    }

    virtual void clearPortUpdate(const std::string& portName) {
        generic_->portsUpdated_[portName] = false;
    }

    virtual bool isPortUpdated(const std::string& portName) {
        return generic_->portsUpdated_[portName];
    }

    template <typename T>
    void sendPacket(const T& event, const std::string& portName) {
        //auto port = std::dynamic_pointer_cast<Port<bundle::ValidReady>>(getPort(portName));
        auto port = getPort(portName);
        port->send(event);
        std::cout << "ComponentSrc sending event: data = " << event.data << ", valid = " << event.valid << std::endl;
    }

    // 时钟更新
    virtual void tick() {
        std::cout << "Updating clock for component" << std::endl;
    }
};

// 注册组件类型
REFL_AUTO(
    type(Component),
    field(generic_)
)

REGISTER_OBJECT(Component)
#endif // COMPONENT_H

