#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "TypeManager.h"
#include "Registry.h"

class ObjectBuildVisitor : public Visitor<void> {
public:
    explicit ObjectBuildVisitor(TypeManager& typeManager, Registry& registry) : typeManager_(typeManager), registry_(registry) {}

    void visit(Node& node) override {
        for (const auto& [typeName, properties] : node.getProperties()) {
            if (!node.getObject(typeName)) {
                auto instance = registry_.createObjectByName(typeName);
                typeManager_.setProperties(typeName, instance, properties);
                node.setObject(typeName, instance);
            }
        }
        Visitor<void>::visit(node);
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}
private:
    TypeManager& typeManager_;
    Registry& registry_;
};

