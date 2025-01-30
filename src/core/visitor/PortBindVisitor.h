#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "Wire.h"

class PortBindVisitor : public Visitor<void> {
public:
    explicit PortBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(EntityRef entity, int level) override {
        auto wireObject = entity.getObject("Wire");
        if (wireObject) {
            auto wire = std::static_pointer_cast<Wire>(wireObject);
            wire->bind();
        }

        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef) override {}

private:
    Tree& tree_;
};

