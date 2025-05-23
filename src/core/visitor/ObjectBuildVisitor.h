#ifndef OBJECT_BUILD_VISITOR_H
#define OBJECT_BUILD_VISITOR_H

#include "Visitor.h"
#include "Path.h"
#include "TypeManager.h"

class ObjectBuildVisitor : public Visitor<void> {
public:
    explicit ObjectBuildVisitor(TypeManager& typeManager) : typeManager_(typeManager) {}

    void visit(Path entity, int level) override {
        std::cout << "ObjectBuilder visit enttiy:" << entity.getEntityPath() << " at level=" << level << ":\n";
        for (const auto& [type_name, serialize] : entity.getSerializies()) {
            if (!entity.getObject(type_name).ptr()) {
                // create obj from serialize first
                //auto&& obj = typeManager_.createObject(type_name, serialize);
                // create obj in StoragePool
                std::cout << "ObjectBuilder entry deserialize object: " << entity.getEntityPath() << "\n";
                //auto instance = registry_->createObjectByName(type_name, std::make_optional(std::cref(serialize)));
                auto instance = typeManager_.createStorageObject(type_name, entity, std::make_optional(std::cref(serialize)));
                entity.setObject(type_name, instance);
            //} else {
                // throw std::runtime_error("object is aready created, should not be restore from json");
            }
        }
        for (const auto& [type_name, properties] : entity.getProperties()) {
            if (!entity.getObject(type_name).ptr()) {
                std::cout << "ObjectBuilder create object: " << entity.getEntityPath() << ":" << type_name->str() << "\n";
                //auto instance = registry_->createObjectByName(type_name);
                auto instance = typeManager_.createStorageObject(type_name, entity, std::nullopt);
                typeManager_.setProperties(type_name, instance, properties);
                entity.setObject(type_name, instance);
            } else {
                std::cout << "ObjectBuilder set object: " << entity.getEntityPath() << ":" << type_name->str() << " property\n";
                // object is created, set member property
                for (const auto& [member_name, property]: properties) {
                    entity.setProperty(type_name, member_name, property);
                }
            }
        }
        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef, Path) override {}
private:
    TypeManager& typeManager_;
    //std::shared_ptr<Registry> registry_;
};

#endif // OBJECT_BUILD_VISITOR_H
