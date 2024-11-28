#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <new> // std::launcher
#include <cassert>
#include "Object.h"

template<typename ObjectType, typename EntityType = EntityNull>
class Storage {
    using SelfType = Storage<ObjectType, EntityType>;
    using SelfPtr = std::shared_ptr<SelfType>;
public:
        // static const size_t initialSize = 64;
    // static_assert(std::is_base_of<Object, ObjectTtype>::value, "T must be a subclass of Object");
    using StorageType = typename std::aligned_storage<sizeof(ObjectType), alignof(ObjectType)>::type;
    using ObjectPtr = typename std::shared_ptr<ObjectType>;
    using ObjectWeakPtr = typename std::weak_ptr<ObjectType>;
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
        if (!freeList.empty()) {
            expandStorage();
        }

        auto& free_id = freeList.front();
        // 使用std::launder来重新解释转换指针，确保正确处理可能存在的别名。
        ObjectType* object = std::launder(reinterpret_cast<ObjectType*>(&poolStorage[free_id]));
        freeList.pop();

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
        ObjectType* p = std::launder(reinterpret_cast<ObjectType*>(&poolStorage[id]));
        if (activePtr.count(p) > 0) { 
            auto obj_ptr = activePtr[p].lock();
            assert(obj_ptr);
            return obj_ptr;
        }

        auto ptr = ObjectPtr(p, [weak_this = std::weak_ptr<SelfType>(self)](ObjectType* ptr) {
            if (auto storage_ptr = weak_this.lock()) {
                storage_ptr->release(ptr);
            // } else {
            //    ptr->~ObjectType();  pool is already destroy, no need to destroy 
            }
        });
        activePtr.insert({p, std::weak_ptr<ObjectType>(ptr)});
    }

        // get objectId from shared_ptr
    ObjectId getObjectId(const ObjectPtr& ptr) const {
        const auto& p = ptr.get();
        return getObjectId(p);
    }
    ObjectId getObjectId(const ObjectType* p) const {
        auto diff = static_cast<std::ptrdiff_t>(reinterpret_cast<const char*>(p) - reinterpret_cast<const char*>(poolStorage.data()));
        size_t index = diff / sizeof(StorageType);
        return (ObjectId)index;
    }
    void addObject(EntityId entity, ObjectPtr ptr) {
        static_assert(!std::is_same<EntityType, EntityNull>::value, " EntityNull is not allowd to add object");
        if (entityToIndex.find(entity) != entityToIndex.end()) {
            throw std::runtime_error("Can't add since Entity is already add this Object");
        }
        ObjectPtr obj_ptr;
        if (activePtr.count(ptr.get())) {
            obj_ptr = ptr;
        } else {
            // if input Object is created outside of this storage
            obj_ptr = create(ptr);
        }
        ObjectId id = getObjectId(obj_ptr.get());
        entityToIndex[entity] = id;
        indexToEntity[id] = entity;
    }

    template<typename... Args>
    void addObject(EntityId entity, Args&&... args) {
        static_assert(!std::is_same<EntityType, EntityNull>::value, " EntityNull is not allowd to add object");
        auto obj_ptr = create(std::forward<Args>(args)...);
        addObject(entity, obj_ptr);
    }
    void removeObject(EntityId entity) {
        static_assert(!std::is_same_v<EntityType, EntityNull>, " EntityNull is not allowd to add object");
        if (entityToIndex.find(entity) == entityToIndex.end()) {
            throw std::runtime_error("Can't remove since Entity don't add this Object");
        }
        ObjectId removed_id = entityToIndex[entity];
        entityToIndex.erase(entity);
        indexToEntity.erase(removed_id);
    }
    ObjectPtr& getObject(EntityId entity) {
        if (entityToIndex.find(entity) == entityToIndex.end()) {
            throw std::runtime_error("Can't remove since Entity don't add this Object");
        }
        ObjectId id = entityToIndex[entity];
        return getSharedPtrFromIndex(id);
    }

    template<typename... Args>
    ObjectPtr& getOrCreateObject(EntityId entity, Args&&... args) {
        if (entityToIndex.find(entity) == entityToIndex.end()) {
            addObject(entity, std::forward<Args>(args)...);
        }
        return getObject(entity);
    }
private:
    std::vector<StorageType> poolStorage;

    std::queue<size_t> freeList;
    std::map<ObjectType*, ObjectWeakPtr>   activePtr;       // 
    SelfPtr self;

    void release(ObjectType* obj_p) {
        obj_p->~ObjectType(); // 显式调用析构函数，以重置对象状态

        ObjectId id = getObjectId(obj_p);
        freeList.push(id);
        activePtr.erase(obj_p);
    }
    void expandStorage() {
        size_t current_size = poolStorage.size();
        size_t new_size = current_size + PageSize;
        //cassert(new_size < ReserveSize);
        //static_assert(new_size < ReserveSize);
        poolStorage.resize(new_size);

        for (size_t i = current_size; i < new_size; ++i) {
            void* new_space = &poolStorage[i];
            freeList.push(reinterpret_cast<size_t>(new_space));
        }
    }
    // Iterator...
};
