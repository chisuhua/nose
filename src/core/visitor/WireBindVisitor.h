#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "Wire.h"

class WireBindVisitor : public Visitor<void> {
public:
    explicit WireBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(Path entity, int level) override {
        auto wireObject = entity.getObject("Wire");
        if (wireObject.ptr()) {
            auto wire = wireObject.as<Wire>();
            wire->bind();
        }

        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef, Path) override {}

private:
    Tree& tree_;
};

