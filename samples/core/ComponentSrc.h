#include <Component.h>

class ComponentSrc : public Component {
public:
    ComponentSrc() = default;

    void sendEvent(const Event& event, const std::string& portName) {
        auto port = std::dynamic_pointer_cast<Port<IOType<Event, RSP>>>(getPort(portName));
        if (port) {
            *port << event;
            std::cout << "ComponentSrc sending event: data = " << event.data << ", valid = " << event.valid << std::endl;
        }
    }

    void tick() override {
        Component::tick();
        std::cout << "ComponentSrc clock update" << std::endl;
    }
};

