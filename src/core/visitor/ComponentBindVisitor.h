// ComponentBindVisitor.hpp
#include "Visitor.hpp"
#include "Tree.hpp"
#include "Component.hpp"

class ComponentBindVisitor : public Visitor<void> {
public:
    void visit(Node& node) override {
        auto component = std::dynamic_pointer_cast<Component>(node.getObject(node.getName()));
        if (component) {
            for (const auto& [key, childNode] : node.getChildren()) {
                auto portNode = childNode->getObject("Port");
                if (portNode) {
                    component->addPort(key, std::static_pointer_cast<IPort>(portNode));

                    if (port->role == Role::Slave) {
                        auto portNotify = [component, key]() {
                            component->portNotified(key);
                        };
                        port->addObserver(portNotify);
                    }
                }
            }
        }

        for (const auto& [key, child] : node.getChildren()) {
            visit(*child);
        }
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}
};

