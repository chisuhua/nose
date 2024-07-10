// ObjectBuildVisitor.hpp
#include "Visitor.hpp"
#include "Tree.hpp"
#include "TypeManager.hpp"

class ObjectBuildVisitor : public Visitor<void> {
public:
    explicit ObjectBuildVisitor(TypeManager& typeManager) : typeManager_(typeManager) {}

    void visit(Node& node) override {
        for (const auto& [typeName, properties] : node.getProperties()) {
            if (!node.getObject(typeName)) {
                auto instance = typeManager_.create(typeName, properties);
                node.setObject(typeName, instance);
            }
        }

        for (const auto& [key, child] : node.getChildren()) {
            visit(*child);
        }
    }

private:
    TypeManager& typeManager_;
};
