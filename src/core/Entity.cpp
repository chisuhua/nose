#include "Entity.h"
#include "Visitor.h"




void Entity::accept(Visitor<void>& visitor) {
    //for (const auto& [key, child] : children_) {
        //child->accept(visitor);
    //}
    //for (const auto& [key, object] : objects_) {
        //visitor.visitObject(object, key);
    //}
    visitor.visit(*this);
}
