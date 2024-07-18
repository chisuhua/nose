#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "Component.h"

class ComponentBindVisitor : public Visitor<void> {
public:
    void visit(Node& node) override {
        auto component = std::static_pointer_cast<Component>(node.getObject(node.getName()));
        if (component) {
            for (const auto& [key, childNode] : node.getChildren()) {
                auto port_node = childNode->getObject("Port");
                if (port_node) {
                    component->addPort(key, std::static_pointer_cast<IPort>(port_node));

                    //if (port->role == Role::Slave) {
                        //auto portNotify = [component, key]() {
                            //component->portNotified(key);
                        //};
                        //port->addObserver(portNotify);
                    //}
                }
            }
        }
        Visitor<void>::visit(node);
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}
};

