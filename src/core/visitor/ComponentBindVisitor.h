#ifndef COMPONENT_BIND_VISITOR_H
#define COMPONENT_BIND_VISITOR_H
#include "Visitor.h"
#include "Tree.h"
#include "Component.h"
#include "Port.h"

class ComponentBindVisitor : public Visitor<void> {
public:
    //explicit ComponentBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(EntityRef entity, int level) override {
        auto component = std::static_pointer_cast<Component>(entity.getObject(entity.getName()));
        if (component) {
            for (const auto& [key, childEntity] : entity.getChildren()) {
                auto childEntityRef = EntityRef(childEntity);
                auto port = childEntityRef.make_object<Port>();
                //if (port_entity) {
                    component->addPort(key, port);

                    //if (port->role == Role::Slave) {
                        //auto portNotify = [component, key]() {
                            //component->portNotified(key);
                        //};
                        //port->addObserver(portNotify);
                    //}
                //}
            }
        }
        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef) override {}

private:
    //Tree& tree_;
};
#endif
