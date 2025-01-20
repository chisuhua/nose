#ifndef VISITOR_H
#define VISITOR_H

#include <memory>
#include <string>
#include "EntityIntern.h"
#include "StringIntern.h"

template<typename T>
class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(EntityRef entity, int level) {
        for (const auto& [key, child] : entity.getChildren()) {
            child->accept(*this, level + 1);
        }
        for (const auto& [key, object] : entity.getObjects()) {
            visitObject(object, key);
        }
    };

    //virtual void visit(ConstEntityRef entity, int level) {
        //for (const auto& [key, child] : entity.getChildren()) {
            //// 假设 getChildren 返回的是 ConstEntityRef 类型
            //child->accept(*this, level + 1); // 现在 accept 是 const 成员函数，因此可以在这里调用
        //}
        //for (const auto& [key, object] : entity.getObjects()) {
            //visitObject(object, key);
        //}
    //}

    virtual void visitObject(const std::shared_ptr<void>&, StringRef) = 0;
};

#endif // VISITOR_H

