#ifndef COMPONENTDST_H
#define COMPONENTDST_H

#include "Component.h"

class ComponentDst : public Component {
public:
    using GenericType = std::shared_ptr<ComponentData>;

    explicit ComponentDst(GenericType generic) 
        : Component(generic) {}


    void portNotified(const std::string& portName) override {
        Component::portNotified(portName);
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
