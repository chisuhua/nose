#ifndef REGISTRY_H
#define REGISTRY_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "Storage.h"
#include "Object.h"
#include "TypeInfo.h"
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
        auto it = objectStorage.find(ti.Name());
        if (it == objectStorage.end()) {
            auto storage = std::make_shared<StorageType>();
            objectStorage[ti.Name()] = std::static_pointer_cast<void>(storage);
            return storage;
        }
        return std::static_pointer_cast<StorageType>(it->second);
    }

    template<typename O, typename E>
    void registerObject(const std::string& type_tag) {
        StringRef type_name = TypeManager::instance().registerType<O>(type_tag);

        if constexpr(has_generic_v<O>) {
            using GenericType = typename O::GenericType::element_type;
            Registry::getInstance()->registerObject<GenericType, E>(type_tag + "_GenericType");
            // Object with has_generic_v is only created from generic object
            //StorageObjectCreator creator =  [this](Path entity, std::any arg) -> std::shared_ptr<void> {
                    //auto generic = std::any_cast<std::shared_ptr<GenericType>>(arg);
                    //return std::static_pointer_cast<void>(getOrCreateObject<O, E>(entity, generic));
                //};
            StorageObjectCreator_t<const VoidPtrType&> generic_creator = [this](Path path, const VoidPtrType& generic) -> ObjRef {
                    return getOrCreateObject<O, E>(path.ptr(), std::static_pointer_cast<GenericType>(generic));
                };
            TypeManager::instance().registerStorageObjectCreator(type_name, generic_creator);

        } else {
            // Generic object create from rfl_generic
            StorageObjectCreator_t<GenericRef> rfl_generic_creator = [this](Path path, GenericRef rfl_generic) -> ObjRef {
                    auto& obj_from_rfl = rfl::from_generic<O>(rfl_generic).value();
                    return getOrCreateObject<O, E>(path.ptr(), obj_from_rfl);
                };
            TypeManager::instance().registerStorageObjectCreator(type_name, rfl_generic_creator);

            StorageObjectCreator_t<std::nullopt_t> default_creator = [this](Path path, std::nullopt_t ) -> ObjRef {
                    return getOrCreateObject<O, E>(path.ptr());
                };
            TypeManager::instance().registerStorageObjectCreator(type_name, default_creator);

            // Generic object created by copy constructor
            StorageObjectCreator_t<O> copy_creator = [this](Path path, O&& other) -> ObjRef {
                    //auto other = std::any_cast<O>(arg);
                    return getOrCreateObject<O, E>(path.ptr(), other);
                };
            TypeManager::instance().registerStorageObjectCreator(type_name, copy_creator);
        }

    }

public:
    //template<typename O, typename E, typename... Args>
    //std::shared_ptr<O> createObject(Args&&... args) {
        ////static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        //getStorage<O, E>()->create(std::forward<Args>(args)...);
    //}

    template<typename O, typename E, typename... Args>
    ObjRef getOrCreateObject(EntityPtr entity, Args&&... args) {
        // TODO has_generic_v and  is_reflectable
        //static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        auto obj =  getStorage<O, E>()->getOrCreateObject(entity->getHash(), std::forward<Args>(args)...);
        return ObjRef(TypeInfo::getTypeName<O>(), obj);
    }

    template<typename O, typename E>
    std::shared_ptr<O> getObject(EntityPtr entity) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        return getStorage<O, E>()->getObject(entity->getHash());
    }

    template<typename O, typename E, typename... Args>
    void addObject(EntityPtr entity, Args&&... args) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        getStorage<O, E>()->addObject(entity->getHash(), std::forward<Args>(args)...);
    }
    //template<typename O>
    //auto getObjectId(std::shared_ptr<O> obj_ptr) -> O::ObjectId {
        //static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        //return getStorage<O, E>()->getObjectId(obj_ptr);
    //}
    template<typename O, typename E>
    void removeObject(EntityPtr entity) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        getStorage<O, E>()->removeObjectByEntity(entity->getHash());
    }

    template<typename O, typename E>
    void removeObject(std::shared_ptr<O> obj) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        typename E::ObjectId object_id = getStorage<E>()->getObjectId(obj);
        getStorage<O, E>()->removeObjectByObjectId(object_id);
    }

};

// 用于全局注册的工具模板类
template <typename O, typename E = Entity>
struct ObjectRegistrar {
    ObjectRegistrar(const std::string& name) {
        Registry::getInstance()->registerObject<O, E>(name);
    }
};

#define REGISTER_OBJECT_1(name) static ObjectRegistrar<name> auto_##name(#name);
#define REGISTER_OBJECT_2(name, str) static ObjectRegistrar<name> auto_##name(str);

// 主宏，根据参数数量选择合适的宏
#define GET_MACRO(_1,_2,NAME,...) NAME
#define REGISTER_OBJECT(...) GET_MACRO(__VA_ARGS__, REGISTER_OBJECT_2, REGISTER_OBJECT_1)(__VA_ARGS__)

#define REGISTER_NODE_OBJECT(O, ...) \
    do { \
        REFL_AUTO(__VA_ARGS__) \
        static ObjectRegistrar<O, Entity> auto_##O##_registrar(); \
    } while(0)

//#define STRINGIFY_HELPER(...) #__VA_ARGS__
//#define STRINGIFY(x) STRINGIFY_HELPER x

//// 辅助宏，用于选择第一个参数
//#define FIRST_ARG_IMPL(_1, ...) auto_##_1

//#define REGISTER_OBJECT(O, ...) static ObjectRegistrar<O, EntityNull, __VA_ARGS__> auto_##O##_registrar;
//#define REGISTER_OBJECT(O, ...) static ObjectRegistrar<O> FIRST_ARG_IMPL(O)(__VA_ARGS__);
//#define REGISTER_OBJECT(O, ...) static ObjectRegistrar<O> auto_##O##_ (__VA_ARGS__);
//#define REGISTER_OBJECT(O, T) static ObjectRegistrar<O> auto_##O##_ (##T);

//#define REGISTER_OBJECT(...) static ObjectRegistrar<__VA_ARGS__> STRINGIFY((__VA_ARGS_))##_registrar;
#endif
