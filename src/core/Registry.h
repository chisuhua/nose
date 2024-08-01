#pragma once
#include <map>
#include <functional>
#include <string>
#include <utility>
#include "Storage.h"
#include "TypeManager.h"

class Registry {
public:
    static std::shared_ptr<Registry> getInstance() {
        static std::shared_ptr<Registry> instance(new Registry());
        return instance;
    }

    Registry() = default;
    
public:
    std::unordered_map<std::string, std::shared_ptr<void>> objectStorage;
    std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> objectConstructor;
    //std::unordered_map<std::string, refl::type_descriptor> registeredTypeDesc;
    //std::vector<std::string> registeredTypeNames;

    template<typename O, typename E>
    auto getStorage() -> Storage<O, E>* {
        static_assert(refl::trait::is_reflectable<O>::value, "T must be a reflectable type.");
        using StorageType = Storage<O, E>;

        constexpr auto& ti = TypeInfo::Get<::refl::trait::remove_qualifiers_t<StorageType>>();

        auto it = objectStorage.find(ti.Name());
        if (it == objectStorage.end()) {
            auto storage = std::make_shared<StorageType>();
            objectStorage[ti.Name()] = std::static_pointer_cast<void>(storage);
            return storage.get();
        }
        return std::static_pointer_cast<StorageType>(it->second.get());
    }
    template<typename O, typename E>
    void registerObject(const std::string& type_name) {
        auto result = objectConstructor.try_emplace(type_name, [](auto... args) -> std::shared_ptr<void> {
            return std::static_pointer_cast<void>(getStorage<O, E>()->create(args...));
        });
    }

    template<typename... Args>
    std::shared_ptr<void> createObjectByName(const std::string& type_name, Args&... args) {
        auto it = objectConstructor.find(type_name);
        if(it != objectConstructor.end()) {
            return std::static_pointer_cast<void>(it->second(std::forward<Args>(args)...));
        }
        return nullptr;
    }
public:
    template<typename O, typename E, typename... Args>
    std::shared_ptr<O> createObject(Args&&... args) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        getStorage<O, E>()->create(std::forward<Args>(args)...);
    }
    template<typename O, typename E, typename... Args>
    std::shared_ptr<O> getOrCreateObject(std::shared_ptr<E> entity, Args&& ... args) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        typename E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->getOrObject(entity_id, std::forward<Args>(args)...);
    }
    template<typename O, typename E>
    std::shared_ptr<O> getObject(std::shared_ptr<E> entity) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        typename E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->getObject(entity_id);
    }
    template<typename O, typename E, typename... Args>
    void addObject(std::shared_ptr<E> entity, Args&&... args) {
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
    void removeObject(std::shared_ptr<E> entity) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        typename E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->removeObject(entity_id);
    }
};

// 用于全局注册的工具模板类
template <typename O, typename E>
struct ObjectRegistrar {
    ObjectRegistrar() {
        auto& type_name = TypeManager::instance().registerType<O>();
        Registry::getInstance()->registerObject<O, E>(type_name);
    }
};

#define REGISTER_NODE_OBJECT(O, ...) \
    do { \
    REFL_AUTO(__VA_ARGS__) \
    static ObjectRegistrar<O, Entity> auto_##O##_registrar(); \
    }  while(0) 

#define REGISTER_OBJECT(O) static ObjectRegistrar<O, EntityNull> auto_##O##_registrar(); 
