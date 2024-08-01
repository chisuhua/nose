#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "TypeManager.h"
#include "Registry.h"

class ObjectBuildVisitor : public Visitor<void> {
public:
    explicit ObjectBuildVisitor(TypeManager& typeManager, std::shared_ptr<Registry> registry) : typeManager_(typeManager), registry_(registry) {}

    void visit(Entity& entity) override {
        for (const auto& [type_name, properties] : entity.getProperties()) {
            if (!entity.getObject(type_name)) {
                auto instance = registry_->createObjectByName(type_name);
                typeManager_.setProperties(type_name, instance, properties);
                entity.setObject(type_name, instance);
            }
        }
        Visitor<void>::visit(entity);
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}
private:
    TypeManager& typeManager_;
    std::shared_ptr<Registry> registry_;
};

