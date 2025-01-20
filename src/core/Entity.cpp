#include "Entity.h"
#include "Registry.h"
#include "TypeManager.h"
#include "Visitor.h"
#include <optional>




void Entity::accept(Visitor<void>& visitor, int level) const {
    //visitor.visit(std::static_pointer_cast<Entity>(shared_from_this()), level);
    visitor.visit(shared_from_this(), level);
}

//template <typename T, typename E, typename... Args>
//std::shared_ptr<T> Entity::getOrCreateObject(Args&&... args) {
    //auto obj = getObject<T>();
    //if (obj == nullptr) {
        //Registry::getInstance()->getOrCreateObject<T>(shared_from_this(), std::forward<Args>(args)...);
    //}
    //return obj;
//}

std::shared_ptr<void> Entity::getOrCreateObject(StringRef type_name, std::optional<GenericRef> rfl_generic) {
    auto obj = getObject(type_name);
    if (obj) return obj;
    objects_[type_name] = TypeManager::instance().createStorageObject(type_name, EntityRef(shared_from_this()), rfl_generic);
    return objects_[type_name];
}

void Entity::deserialize(StringRef type_name) {
    auto generic = objectsInSerialize_[type_name];
    objects_[type_name] = TypeManager::instance().createStorageObject(type_name, EntityRef(shared_from_this()), std::make_optional(generic));
}

//template <typename T>
//void Entity::setObject(const std::shared_ptr<T>& object) {
    //auto type_name = TypeManager::instance().getTypeName<T>();
    //objects_[type_name] = std::static_pointer_cast<void>(object);
//}

//template <typename T>
//std::shared_ptr<T> Entity::getObject() const {
    //auto type_name = TypeManager::instance().getTypeName<T>();
    //auto it = objects_.find(type_name);
    //if (it != objects_.end()) {
        //return std::static_pointer_cast<T>(it->second);
    //}
    //return nullptr;
//}

