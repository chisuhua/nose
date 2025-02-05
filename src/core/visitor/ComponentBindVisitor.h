#ifndef COMPONENT_BIND_VISITOR_H
#define COMPONENT_BIND_VISITOR_H
#include "Visitor.h"
#include "Tree.h"
#include "Component.h"
#include "Port.h"
#include <memory>

class ComponentBindVisitor : public Visitor<Component> {
public:
    //explicit ComponentBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(Path entity, int level) override {
        //std::cout << "\nenter visit path " << entity.getEntityPath() << "\n";
        Visitor<Component>::visit(entity, level);
        //std::cout << "leave visit path " << entity.getEntityPath() << "\n";
    }

    void visitObject(const std::shared_ptr<Component>& obj, StringRef name, Path obj_path) override {
        std::cout << "visitObject type_name: " << name->str() << " at path:" << obj_path.getEntityPath() << "\n";
        auto component = std::static_pointer_cast<Component>(obj);
        //using ObjType = decltype(obj);
        //std::cout << "ObjType is " << TypeInfo::getTypeName<ObjType>()->str() << "\n";
        if (component) {
            std::cout << "found component for " << obj_path.getName() << "\n";
            for (const auto& [key, childEntity] : obj_path.getChildren()) {
                std::cout << "  component's sub entity  for " << key << "for child " << childEntity->getName() << "\n";
                auto port = std::static_pointer_cast<Port>(childEntity->getObject<Port>());
                if (port) {
                    auto obj_port = ObjPtr<Port>::make(port);
                    component->addPort(key, obj_port);
                    auto portNotify = [component, key]() {
                        component->updatePort(key);
                    };
                    port->addObserver(portNotify);
                }
            }
        }
    }

private:
    //Tree& tree_;
};
#endif
