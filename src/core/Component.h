#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include "Registry.h"
#include "Port.h"

class Component {
public:
    // 端口映射
    std::map<std::string, std::shared_ptr<Port>> ports_;
    // 端口更新标志映射
    std::map<std::string, bool> portsUpdated_;

    virtual ~Component() = default;

    // 添加端口
    void addPort(const std::string& name, std::shared_ptr<Port> port) {
        ports_[name] = port;
    }

    // 获取端口
    std::shared_ptr<Port> getPort(const std::string& name) {
        auto it = ports_.find(name);
        if (it != ports_.end()) {
            return it->second;
        }
        return nullptr;
    }

    // 端口通知
    virtual void portNotified(const std::string& portName) {
        portsUpdated_[portName] = true;
    }

    // 时钟更新
    virtual void tick() {
        std::cout << "Updating clock for component" << std::endl;
    }
};

// 注册组件类型
REFL_AUTO(
    type(Component),
    field(ports_),
    field(portsUpdated_)
)

#endif // COMPONENT_H

