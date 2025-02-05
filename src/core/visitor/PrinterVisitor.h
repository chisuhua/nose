#ifndef PRINTER_VISITOR_H
#define PRINTER_VISITOR_H

#include "Visitor.h"
#include "Tree.h"
#include "Component.h"

class PrinterVisitor : public Visitor<void> {
public:
    PrinterVisitor() {}
    virtual ~PrinterVisitor() = default;

    void visit(Path entity, int level) override {
        printEntity(entity, level);
        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef, Path) override {}

private:
    void printEntity(Path entity, int level) {
        std::string indent(level * 2, ' ');
        std::cout << indent << entity.getName() << "\n";
    }
};

#endif // PRINTER_VISITOR_H

