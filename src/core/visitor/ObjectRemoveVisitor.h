#ifndef OBJECT_REMOVE_VISITOR_H
#define OBJECT_REMOVE_VISITOR_H

#include "Visitor.h"
#include "Path.h"

class ObjectRemoveVisitor : public Visitor<void> {
public:
    explicit ObjectRemoveVisitor() {}

    void visit(Path entity, int level) override {
        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef type_name, Path path) override {
        path.removeObject(type_name);
    }
private:
};

#endif // OBJECT_REMOVE_VISITOR_H
