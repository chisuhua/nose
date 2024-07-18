#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "Component.h"

// class PrinterVisitor : public Visitor<CustomObject, ExprObject, IntObject> {
class PrinterVisitor : public Visitor<void> {
public:
    PrinterVisitor() {}
    virtual ~PrinterVisitor() = default;

    void visit(Node& node) override {
        printNode(node, 0);
        Visitor<void>::visit(node);
    }

    virtual void visitObject(const std::shared_ptr<void>& obj, const std::string& key) override {
    }

    //void Visit(const std::shared_ptr<CustomObject>& obj) override {
        //std::cout << "CustomObject: intval = " << obj->intval
                  //<< ", floatval = " << obj->floatval << std::endl;
    //}

    //void Visit(const std::shared_ptr<ExprObject>& obj) override {
        //std::cout << "ExprObject: expression = " << obj->expression << std::endl;
    //}

    //void Visit(const std::shared_ptr<IntObject>& obj) override {
        //std::cout << "IntObject: value = " << obj->value << std::endl;
    //}

private:
    void printNode(Node& node, int level) {
        std::string indent(level * 2, ' ');
        std::cout << indent << node.getName() << "\n";
    }
};

