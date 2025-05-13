#ifndef COMPONENTDST_H
#define COMPONENTDST_H

#include "Component.h"

//struct ComponentDstData : public ComponentData {
//    uint64_t testdat_only;
//};
//struct ComponentDstData {
//    // 端口映射
//    std::map<std::string,ObjPtr<Port>> ports_;
//    // 端口更新标志映射
//    std::map<std::string, bool> portsUpdated_;
//
//    uint64_t test_only;
//};

class ComponentDst : public Component {
public:
    using GenericType = std::shared_ptr<ComponentData>;

    explicit ComponentDst(GenericType generic) 
        : Component(generic) {}


    void updatePort(const std::string& portName) override {
        Component::updatePort(portName);
        std::cout << "ComponentDst: Port " << portName << " was updated." << std::endl;
    }

    void tick() override {
        Component::tick();
        std::cout << "ComponentDst clock update" << std::endl;
    }
};

REFL_AUTO(
    type(ComponentDst, bases<Component>)
    );

REGISTER_OBJECT(ComponentDst)
#endif
