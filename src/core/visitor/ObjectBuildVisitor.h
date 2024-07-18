#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "TypeManager.h"
#include "Registry.h"

class ObjectBuildVisitor : public Visitor<void> {
public:
    explicit ObjectBuildVisitor(TypeManager& typeManager, std::shared_ptr<Registry> registry) : typeManager_(typeManager), registry_(registry) {}

    void visit(Node& node) override {
        for (const auto& [type_name, properties] : node.getProperties()) {
            if (!node.getObject(type_name)) {
                auto instance = registry_->createObjectByName(type_name);
                typeManager_.setProperties(type_name, instance, properties);
                node.setObject(type_name, instance);
            }
        }
        Visitor<void>::visit(node);
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}
private:
    TypeManager& typeManager_;
    std::shared_ptr<Registry> registry_;
};

