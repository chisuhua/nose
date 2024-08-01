#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "Component.h"

class ComponentBindVisitor : public Visitor<void> {
public:
    void visit(Entity& entity) override {
        auto component = std::static_pointer_cast<Component>(entity.getObject(entity.getName()));
        if (component) {
            for (const auto& [key, childEntity] : entity.getChildren()) {
                auto port_entity = childEntity->getObject("Port");
                if (port_entity) {
                    component->addPort(key, std::static_pointer_cast<IPort>(port_entity));

                    //if (port->role == Role::Slave) {
                        //auto portNotify = [component, key]() {
                            //component->portNotified(key);
                        //};
                        //port->addObserver(portNotify);
                    //}
                }
            }
        }
        Visitor<void>::visit(entity);
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}
};

