// PortBindVisitor.hpp
#include "Visitor.h"
#include "Tree.h"

class PortBindVisitor : public Visitor<void> {
public:
    explicit PortBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(Node& node) override {
        auto wireObject = node.getObject("Wire");
        if (wireObject) {
            auto wire = std::dynamic_pointer_cast<Wire>(wireObject);
            wire->Bind();
        }

        for (const auto& [key, child] : node.getChildren()) {
            visit(*child);
        }
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}

private:
    Tree& tree_;
};

