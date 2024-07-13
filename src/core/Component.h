#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <string>

class Component {
public:
    std::map<std::string, std::shared_ptr<IPort>> ports_;
    std::map<std::string, bool> portsUpdated_;

    virtual ~Component() = default;

    void addPort(const std::string& name, std::shared_ptr<IPort> port) {
        ports_[name] = port;
    }

    std::shared_ptr<IPort> getPort(const std::string& name) {
        auto it = ports_.find(name);
        if (it != ports_.end()) {
            return it->second;
        }
        return nullptr;
    }

    virtual void portNotified(const std::string& portName) {
        portsUpdated_[portName] = true;
    }

    virtual void tick() {
        std::cout << "Updating clock for component" << std::endl;
    }
};


