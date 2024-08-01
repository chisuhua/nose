#pragma once
#include "Visitor.h"
#include "Tree.h"

class PortBindVisitor : public Visitor<void> {
public:
    explicit PortBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(Entity& entity) override {
        auto wireObject = entity.getObject("Wire");
        if (wireObject) {
            auto wire = std::static_pointer_cast<Wire>(wireObject);
            wire->Bind();
        }

        Visitor<void>::visit(entity);
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}

private:
    Tree& tree_;
};

