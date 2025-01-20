#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "EntityIntern.h"
#include "TypeManager.h"

class ObjectBuildVisitor : public Visitor<void> {
public:
    explicit ObjectBuildVisitor(TypeManager& typeManager) : typeManager_(typeManager) {}

    void visit(EntityRef entity, int level) override {
        for (const auto& [type_name, serialize] : entity.getSerializies()) {
            if (!entity.getObject(type_name)) {
                // create obj from serialize first
                //auto&& obj = typeManager_.createObject(type_name, serialize);
                // create obj in StoragePool
                //std::cout << "entry deserialize object: " << entity.getPath() << "\n";
                //auto instance = registry_->createObjectByName(type_name, std::make_optional(std::cref(serialize)));
                auto instance = typeManager_.createStorageObject(type_name, entity, std::make_optional(std::cref(serialize)));
                entity.setObject(type_name, instance);
            } else {
                throw std::runtime_error("object is aready created, should not be restore from json");
            }
        }
        for (const auto& [type_name, properties] : entity.getProperties()) {
            if (!entity.getObject(type_name)) {
                //std::cout << "entry set prroperty object: " << entity.getPath() << "\n";
                //auto instance = registry_->createObjectByName(type_name);
                auto instance = typeManager_.createStorageObject(type_name, entity, std::nullopt);
                typeManager_.setProperties(type_name, instance, properties);
                entity.setObject(type_name, instance);
            } else {
                // object is created, set member property
                for (const auto& [member_name, property]: properties) {
                    entity.setProperty(type_name, member_name, property);
                }
            }
        }
        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef) override {}
private:
    TypeManager& typeManager_;
    //std::shared_ptr<Registry> registry_;
};

