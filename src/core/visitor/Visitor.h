#ifndef VISITOR_H
#define VISITOR_H

#include <memory>
#include <string>
#include "Path.h"
#include "TypeManager.h"
#include "StringIntern.h"

template<typename T = void>
class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(Path entity, int level) {
        for (const auto& [key, child] : entity.getChildren()) {
            child->accept(*this, level + 1);
        }
        for (const auto& [key, object] : entity.getObjects()) {
            auto obj_typetag = TypeManager::instance().getTypeTag(key);
            if (TypeInfo::getTypeName<T>() == obj_typetag) {
                auto type_obj = std::static_pointer_cast<T>(object);
                visitObject(type_obj, key, entity);
            }
        }
    };

    //virtual void visit(ConstPath entity, int level) {
        //for (const auto& [key, child] : entity.getChildren()) {
            //// 假设 getChildren 返回的是 ConstPath 类型
            //child->accept(*this, level + 1); // 现在 accept 是 const 成员函数，因此可以在这里调用
        //}
        //for (const auto& [key, object] : entity.getObjects()) {
            //visitObject(object, key);
        //}
    //}

    virtual void visitObject(const std::shared_ptr<T>&, StringRef, Path) = 0;
};

#endif // VISITOR_H

