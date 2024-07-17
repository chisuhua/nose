#pragma once
#include <memory>
#include <string>

class Node;

template<typename T>
class Visitor {
public:
    virtual void visit(Node& node) {
        for (const auto& [key, child] : node.getChildren()) {
            child->accept(*this);
        }
        for (const auto& [key, object] : node.getObjects()) {
            visitObject(object, key);
        }
    };

    virtual void visitObject(const std::shared_ptr<void>&, const std::string&) = 0;
};
