#ifndef STORAGE_H
#define STORAGE_H
#include <vector>
#include <queue>
#include <memory>
#include <map>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <new> // std::launcher
#include <cassert>
#include "TypeInfo.h"

class EntityNull {
public:
    using ObjectId = uint32_t;
    using EntityId = uint32_t;
};


template<typename ObjectType, typename EntityType = EntityNull>
class Storage {
    using SelfType = Storage<ObjectType, EntityType>;
    using SelfPtr = std::shared_ptr<SelfType>;
public:
        // static const size_t initialSize = 64;
    // static_assert(std::is_base_of<Object, ObjectTtype>::value, "T must be a subclass of Object");
    using StorageType = typename std::aligned_storage<sizeof(ObjectType), alignof(ObjectType)>::type;
    //using ObjectPtr = typename std::shared_ptr<ObjectType>;
    using ObjectWeakPtr = typename std::weak_ptr<ObjectType>;
    using ObjectTypePtr = typename std::shared_ptr<ObjectType>;
    using ObjectPtr = VoidPtrType;
    //using ObjectWeakPtr = VoidWeakPtrType;
    using ObjectId = typename EntityType::ObjectId;      // Id is same as object index to storage
    using EntityId = typename EntityType::EntityId;

    std::unordered_map<EntityId, ObjectId> entityToIndex; // EntityId to ObjectId mapping
    std::unordered_map<ObjectId, EntityId> indexToEntity; // ObjectId to EnityId
                                                           //
    static constexpr int ReserveSize = 1024;
    static constexpr int PageSize = 64;
    Storage() : self(SelfPtr(this, [](SelfType* p) {})) {
        poolStorage.reserve(ReserveSize);
        expandStorage();
    }

    ~Storage() {
    }
    template<typename... Args>
    ObjectPtr create(Args&&... args) {
        if (freeList.empty()) {
            expandStorage();
        }

        auto free_id = freeList.front();
        freeList.pop();
        assert(free_id < poolStorage.size());
        // 使用std::launder来重新解释转换指针，确保正确处理可能存在的别名。
        ObjectType* object = std::launder(reinterpret_cast<ObjectType*>(&poolStorage[free_id]));

        // placement new 
        if constexpr (in_place_delete<ObjectType>::value) {
            new(object) ObjectType(std::forward<Args>(args)...);
        } else {
            new(object) ObjectType(std::forward<Args>(args)...);
        }

        return getSharedPtrFromIndex(free_id);
    }

    // create shared_ptr from objectId
    ObjectPtr getSharedPtrFromIndex(ObjectId id) {
        assert(id < poolStorage.size());
        ObjectType* typed_p = std::launder(reinterpret_cast<ObjectType*>(&poolStorage[id]));
        void* p = static_cast<void*>(typed_p);

        if (activePtr.count(p) > 0) { 
            auto obj_ptr = activePtr[p].lock();
            assert(obj_ptr);
            return std::static_pointer_cast<void>(obj_ptr);
        }

        auto ptr = ObjectTypePtr(typed_p, [weak_this = std::weak_ptr<SelfType>(self)](ObjectType* ptr) {
            if (auto storage_ptr = weak_this.lock()) {
                storage_ptr->release(ptr);
            // } else {
            //    ptr->~ObjectType();  pool is already destroy, no need to destroy 
            }
        });
        activePtr.insert({p, std::weak_ptr<ObjectType>(ptr)});
        //activePtr.insert({p, std::weak_ptr<void>(ptr)});
        return std::static_pointer_cast<void>(ptr);
    }

    // get objectId from shared_ptr
    ObjectId getObjectId(ObjectTypePtr ptr) const {
        const auto& p = ptr.get();
        return getObjectId(p);
    }

    ObjectId getObjectId(const ObjectType* p) const {
        auto diff = static_cast<std::ptrdiff_t>(reinterpret_cast<const char*>(p) - reinterpret_cast<const char*>(poolStorage.data()));
        size_t index = diff / sizeof(StorageType);
        return (ObjectId)index;
    }

    void addObjectByPtr(EntityId entity, ObjectPtr& ptr) {
        static_assert(!std::is_same<EntityType, EntityNull>::value, " EntityNull is not allowed to add object");
        if (entityToIndex.find(entity) != entityToIndex.end()) {
            throw std::runtime_error("Can't add since Entity is already added this Object");
        }
        ObjectPtr obj_ptr;
        if (activePtr.count(ptr.get())) {
            obj_ptr = ptr;
        } else {
            // if input Object is created outside of this storage
            auto type_p = std::static_pointer_cast<ObjectType>(ptr);
            obj_ptr = create(*type_p);
        }
        ObjectId id = getObjectId(std::static_pointer_cast<ObjectType>(obj_ptr));
        entityToIndex[entity] = id;
        indexToEntity[id] = entity;
    }

    template<typename... Args>
    ObjectPtr addObject(EntityId entity, Args&&... args) {
        static_assert(!std::is_same<EntityType, EntityNull>::value, "EntityNull is not allowed to add object");
        auto obj_ptr = create(std::forward<Args>(args)...);
        addObjectByPtr(entity, obj_ptr);
        return obj_ptr;
    }

    void removeObjectByEntity(EntityId entity) {
        static_assert(!std::is_same_v<EntityType, EntityNull>, "EntityNull is not allowed to add object");
        if (entityToIndex.find(entity) == entityToIndex.end()) {
            throw std::runtime_error("Can't remove since Entity doesn't add this Object");
        }
        ObjectId removed_id = entityToIndex[entity];
        entityToIndex.erase(entity);
        indexToEntity.erase(removed_id);
    }

    void removeObjectByObjectId(ObjectId id) {
        static_assert(!std::is_same_v<EntityType, EntityNull>, "EntityNull is not allowed to add object");
        if (indexToEntity.find(id) == indexToEntity.end()) {
            throw std::runtime_error("Can't remove since objectId doesn't add this Object");
        }
        EntityId removed_entity = indexToEntity[id];
        entityToIndex.erase(removed_entity);
        indexToEntity.erase(id);
    }


    ObjectPtr getObject(EntityId entity) {
        if (entityToIndex.find(entity) == entityToIndex.end()) {
            throw std::runtime_error("Can't remove since Entity don't add this Object");
        }
        ObjectId id = entityToIndex[entity];
        return getSharedPtrFromIndex(id);
    }

    template<typename... Args>
    ObjectPtr getOrCreateObject(EntityId entity, Args&&... args) {
        if (entityToIndex.find(entity) == entityToIndex.end()) {
            return addObject(entity, std::forward<Args>(args)...);
        }
        return getObject(entity);
    }

private:
    std::vector<StorageType> poolStorage;

    std::queue<size_t> freeList;
    //std::map<ObjectType*, ObjectWeakPtr>   activePtr;       // 
    std::map<void*, ObjectWeakPtr>   activePtr;       // 
    SelfPtr self;

    void release(ObjectType* obj_p) {
        obj_p->~ObjectType();
        void* p = static_cast<void*>(obj_p);

        ObjectId id = getObjectId(obj_p);
        removeObjectByObjectId(id);
        freeList.push(id);
        activePtr.erase(p);
    }
    void expandStorage() {
        size_t current_size = poolStorage.size();
        size_t new_size = current_size + PageSize;
        assert(new_size <= ReserveSize); // 确保新大小不超过 ReserveSize
        //cassert(new_size < ReserveSize);
        //static_assert(new_size < ReserveSize);
        poolStorage.resize(new_size);

        for (size_t i = current_size; i < new_size; ++i) {
            //void* new_space = &poolStorage[i];
            //freeList.push(reinterpret_cast<size_t>(new_space));
            freeList.push(i);
        }
    }
    // Iterator...
};
#endif
