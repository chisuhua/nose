#ifndef OBJECT_REMOVE_VISITOR_H
#define OBJECT_REMOVE_VISITOR_H

#include "Visitor.h"
#include "Path.h"

class ObjectRemoveVisitor : public Visitor<void> {
public:
    explicit ObjectRemoveVisitor() {}

    void visit(Path entity, int level) override {
        removeEntity(entity, level);
        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef type_name, Path path) override {
        std::cout << "ObjectRemove remoteObject type_name: " << type_name->str() << " at path:" << path.getEntityPath() << "\n";
        path.removeObject(type_name);
    }
private:
    void removeEntity(Path entity, int level) {
        std::cout << "ObjectRemove clear path:" << entity.getEntityPath() << "\n";
        entity.clear();
    }

};

#endif // OBJECT_REMOVE_VISITOR_H
