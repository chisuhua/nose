#pragma once
#include "Registry.h"
#include "Component.h"
#include "Event.h"

class ComponentSrc : public Component {
public:
    using GenericType = std::shared_ptr<ComponentData>;

    explicit ComponentSrc(GenericType generic) 
        : Component(generic) {}


    void sendEvent(const Event& event, const std::string& portName) {
        //auto port = std::dynamic_pointer_cast<Port<bundle::ValidReady>>(getPort(portName));
        auto port = getPort(portName);
        port->send(event);
        std::cout << "ComponentSrc sending event: data = " << event.data << ", valid = " << event.valid << std::endl;
    }

    void tick() override {
        Component::tick();
        std::cout << "ComponentSrc clock update" << std::endl;
    }
};

REFL_AUTO(
    type(ComponentSrc, bases<Component>)
    );

REGISTER_OBJECT(ComponentSrc)

