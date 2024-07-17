#include <map>
#include <functional>
#include <string>
#include <utility>
#include <Storage.h>

class Registry {
public:
    static std::shared_ptr<Registry> getInstance() {
        static std::shared_ptr<Registry> instance(new Registry());
        return instance;
    }

    Registry() {
        typeManager = TypeManager::instance();
    }

public:
    std::unordered_map<std::type_index, std::shared_ptr<void>> objectStorage;
    std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> objectConstructor;
    //std::unordered_map<std::string, refl::type_descriptor> registeredTypeDesc;
    //std::vector<std::string> registeredTypeNames;
    TypeManager typeManager;

    template<typename O, typename E>
    auto getStorage() -> Storage<O, E>* {
        static_assert(refl::trait::is_reflectable<T>::value, "T must be a reflectable type.");
        using StorageType = Storage<O, E>;

        std::type_index storage_ti(typeid(StorageType));

        auto it = objectStorage.find(storage_ti);
        if (it == objectStorage.end()) {
            auto storage = std::make_shared<StorageType>();
            objectStorage[storage_ti] = static_pointer_cast<void>(storage);
            return storage.get();
        }
        return static_pointer_cast<StorageType>(it->second.get());
    }
    template<typename O, typename E>
    void registerObjectType(const std::string& type_name) {
        if (not typeManager.findMetadata(type_name)) {
            typeManager.reisterType<O>();
        }

        objectConstructor[type_name] = [](auto... args) -> std::shared_ptr<void> {
            return static_pointer_cast<void>(getStorage<T, E>()->create(args));
        };
        //registeredTypeDesc[type_name] = refl::reflect<T>();
        //registeredTypeNames.push_back(type_name);
    }

    template<typename... Args>
    std::shared_ptr<void> createObjectByName(const std::string& type_name, Args&... args) {
        auto it = objectConstructor.find(typeName);
        if(it != objectConstructor.end()) {
            return static_pointer_cast<void>(it->second(std::forward<Args>(args)...));
        }
        return nullptr;
    }
    //refl::type_descriptor getTypeDescriptor(const std::string& typeName) {
        //auto it = registeredTypeDesc.find(typeName);
        //if(it != registeredTypeDesc.end()) {
            //return it->second;
        //}
        //throw std::runtime_error("Type not found");
    //}
    //const std::vector<std::string>& getRegisteredTypes() const {
                //return registeredTypeNames;
    //}
public:
    template<typename O, typename E, typename... Args>
    std::shared_ptr<O> createObject(Args&& ...) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        getStorage<O, E>()->create(std::forward<Args>(args)...);
    }
    template<typename O, typename E, typename... Args>
    std::shared_ptr<O> getOrCreateObject(std::shared_ptr<E> entity, Args&& ...) {
        static_assert(refl::trait::is_reflectable<T>::value, "Component must be reflectable.");
        E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->getOrObject(entity_id, std::forward<Args>(args)...);
    }
    template<typename O, typename E>
    std::shared_ptr<O> getObject(std::shared_ptr<E> entity) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->getObject(entity_id);
    }
    template<typename O, typename E, typename... Args>
    void addObject(std::shared_ptr<E> entity, Args&&... args) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->addObject(entity, std::forward<Args>(args)...);
    }
    template<typename O>
    auto getObjectId(std::shared_ptr<O> obj_ptr) -> O::ObjectId {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        return getStorage<O, E>()->getObjectId(obj_ptr);
    }
    template<typename O, E>
    void removeObject(std::shared_ptr<E> entity) {
        static_assert(refl::trait::is_reflectable<O>::value, "Component must be reflectable.");
        E::ObjectId entity_id = getStorage<E>()->getObjectId(entity);
        getStorage<O, E>()->removeObject(entity_id);
    }
}
