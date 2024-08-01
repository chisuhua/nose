#pragma once
#include <memory>
#include <string>
#include "Entity.h"

template<typename T>
class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(Entity& entity) {
        for (const auto& [key, child] : entity.getChildren()) {
            child->accept(*this);
        }
        for (const auto& [key, object] : entity.getObjects()) {
            visitObject(object, key);
        }
    };

    virtual void visitObject(const std::shared_ptr<void>&, const std::string&) = 0;
};
