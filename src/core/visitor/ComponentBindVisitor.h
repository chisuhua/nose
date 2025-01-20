#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "Component.h"

class ComponentBindVisitor : public Visitor<void> {
public:
    void visit(EntityRef entity, int level) override {
        auto component = std::static_pointer_cast<Component>(entity.getObject(entity.getName()));
        if (component) {
            for (const auto& [key, childEntity] : entity.getChildren()) {
                auto port_entity = childEntity->getObject("Port");
                if (port_entity) {
                    component->addPort(key, std::static_pointer_cast<Port>(port_entity));

                    //if (port->role == Role::Slave) {
                        //auto portNotify = [component, key]() {
                            //component->portNotified(key);
                        //};
                        //port->addObserver(portNotify);
                    //}
                }
            }
        }
        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef) override {}
};

