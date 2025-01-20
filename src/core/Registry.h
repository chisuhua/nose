#ifndef REGISTRY_H
#define REGISTRY_H
#include <map>
#include <functional>
#include <string>
#include <utility>
#include "Storage.h"
#include "TypeManager.h"
#include "rfl/from_generic.hpp"


class Registry {
public:
    static std::shared_ptr<Registry> getInstance() {
        static std::shared_ptr<Registry> instance(new Registry());
        return instance;
    }

    Registry() = default;

   
public:
    std::unordered_map<StringRef, std::shared_ptr<void>> objectStorage;

    template<typename O, typename E>
    auto getStorage() -> std::shared_ptr<Storage<O, E>> {
        //static_assert(is_reflectable<O>::value, "T must be a reflectable type.");
        using StorageType = Storage<O, E>;

        //constexpr auto& ti = TypeInfo::Get<StorageType>();
        auto& ti = TypeInfo::Get<StorageType>();
        //auto& ti = TypeInfo::Get<::refl::trait::remove_qualifiers_t<StorageType>>();

        auto it = objectStorage.find(ti.Name());
        if (it == objectStorage.end()) {
            auto storage = std::make_shared<StorageType>();
            objectStorage[ti.Name()] = std::static_pointer_cast<void>(storage);
            return storage;
        }
        return std::static_pointer_cast<StorageType>(it->second);
    }

    template<typename O, typename E>
    void registerObject(StringRef type_name) {
        TypeManager::instance().registerStorageObjectCreator(type_name, [this](EntityRef entity, std::optional<GenericRef> rfl_generic) -> std::shared_ptr<void> {
            if (rfl_generic) {
                if constexpr(has_generic_v<O>) {
                    using GenericType = typename O::GenericType;
                    auto generic_obj =  getOrCreateObject<GenericType, E>(entity, rfl::from_generic<GenericType>(rfl_generic.value()).value());
                    return std::static_pointer_cast<void>(getOrCreateObject<O, E>(entity, generic_obj));
                } else {
                    return std::static_pointer_cast<void>(getOrCreateObject<O, E>(entity, rfl::from_generic<O>(rfl_generic.value()).value())); 
                }
            } else return std::static_pointer_cast<void>(getOrCreateObject<O, E>(entity));
        });

    }

public:
    template<typename O, typename E, typename... Args>
    std::shared_ptr<O> createObject(Args&&... args) {
        //static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        getStorage<O, E>()->create(std::forward<Args>(args)...);
    }
    template<typename O, typename E, typename... Args>
    std::shared_ptr<O> getOrCreateObject(EntityRef entity, Args&& ... args) {
        // TODO has_generic_v and  is_reflectable
        //static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        //typename E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->getOrCreateObject(entity.getHash(), std::forward<Args>(args)...);
    }
    template<typename O, typename E>
    std::shared_ptr<O> getObject(EntityRef entity) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        typename E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->getObject(entity_id);
    }
    template<typename O, typename E, typename... Args>
    void addObject(EntityRef entity, Args&&... args) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        typename E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->addObject(entity, std::forward<Args>(args)...);
    }
    //template<typename O>
    //auto getObjectId(std::shared_ptr<O> obj_ptr) -> O::ObjectId {
        //static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        //return getStorage<O, E>()->getObjectId(obj_ptr);
    //}
    template<typename O, typename E>
    void removeObject(EntityRef entity) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        typename E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->removeObject(entity_id);
    }
};

// 用于全局注册的工具模板类
template <typename O, typename E = Entity>
struct ObjectRegistrar {
    ObjectRegistrar() {
        StringRef type_name = TypeManager::instance().registerType<O>();
        Registry::getInstance()->registerObject<O, E>(type_name);
        if constexpr(has_generic_v<O>) {
            using GenericType = typename O::GenericType;
            StringRef generic_type_name = TypeManager::instance().registerType<GenericType>();
            Registry::getInstance()->registerObject<GenericType, E>(generic_type_name);
        }
    }
};

#define REGISTER_NODE_OBJECT(O, ...) \
    do { \
    REFL_AUTO(__VA_ARGS__) \
    static ObjectRegistrar<O, Entity> auto_##O##_registrar(); \
    }  while(0) 

#define STRINGIFY_HELPER(...) #__VA_ARGS__
#define STRINGIFY(x) STRINGIFY_HELPER x

// 辅助宏，用于选择第一个参数
#define FIRST_ARG_IMPL(_1, ...) auto_##_1

//#define REGISTER_OBJECT(O, ...) static ObjectRegistrar<O, EntityNull, __VA_ARGS__> auto_##O##_registrar;
#define REGISTER_OBJECT(...) static ObjectRegistrar<__VA_ARGS__> FIRST_ARG_IMPL(__VA_ARGS__);

//#define REGISTER_OBJECT(...) static ObjectRegistrar<__VA_ARGS__> STRINGIFY((__VA_ARGS_))##_registrar;
#endif
