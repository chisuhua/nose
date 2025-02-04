#ifndef ENTITYPOOL_H
#define ENTITYPOOL_H

#include "Entity.h"
#include "PathUtils.h"
#include "StringIntern.h"
#include <memory>
#include <mutex>

using EntityPtr = std::shared_ptr<Entity>;
using ConstEntityPtr = std::shared_ptr<const Entity>;

using EntityHashType = std::uint32_t;


class EntityPool {
public:
    static std::shared_ptr<EntityPool> getInstance() {
        static std::shared_ptr<EntityPool> instance(new EntityPool());
        return instance;
    }

    EntityPtr getEntity(const std::string& name, const std::string& parent_path) {
        auto full_path = PathUtils::join(parent_path, name);
        return getEntity(full_path);
    }

    EntityPtr getEntity(const std::string& full_path) {
        auto entity_path = PathUtils::getEntityPath(full_path);
        return getEntity(entity_path, fnv1a(entity_path.c_str(), entity_path.length()));
    }

    EntityPtr getEntity(const std::string& full_path, std::uint32_t hash);
    EntityPtr makeEntity(const std::string& name, uint32_t hash) ;

    ~EntityPool() {
        clearPool();
    }

    bool isEntityIntern(const std::string& entity_path) {
        return getEntityByHash(fnv1a_runtime(entity_path.c_str(), entity_path.length())) != nullptr;
    }

    ConstEntityPtr getEntityByHash(std::uint32_t hash) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = pool_.find(hash);
        if (it != pool_.end()) {
            return it->second.lock();
        }
        return nullptr;
    }

private:
    EntityPool() {
        std::string root_name = "/";
        root_entity_ = std::make_shared<Entity>(root_name,  fnv1a(root_name.c_str(), root_name.length()));
    };

    void clearPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!pool_.empty()) {
            auto it = pool_.begin();
            pool_.erase(it); // Erase from the pool map
        }
    }

    std::shared_ptr<Entity> root_entity_;
    std::mutex mutex_;
    std::unordered_map<std::uint32_t, std::weak_ptr<const Entity>> pool_;
};

inline EntityPtr EntityPool::getEntity(const std::string& full_path, std::uint32_t hash) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = pool_.find(hash);
    if (it != pool_.end()) {
        auto locked = it->second.lock();
        if (locked) {
            return std::const_pointer_cast<Entity>(locked);
        }
        return std::shared_ptr<Entity>();
    }

    auto parts = PathUtils::split(full_path);
    auto current_entity = root_entity_;
    for (const auto& part : parts) {
        auto child = current_entity->getOrCreateChild(part);
        current_entity = child;
    }
    return current_entity;
}
 
inline EntityPtr EntityPool::makeEntity(const std::string& name, uint32_t hash) {

    auto deleter = [this, hash](const Entity* p) {
        std::lock_guard<std::mutex> lock(this->mutex_);
        this->pool_.erase(hash);
        delete p;
    };

    auto entity_ref = EntityPtr(new Entity(name, hash), deleter);
    pool_.emplace(hash, std::weak_ptr<Entity>(entity_ref));
    return entity_ref;
}

#endif
