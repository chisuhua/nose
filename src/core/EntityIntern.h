#ifndef ENTITY_INTERN_H
#define ENTITY_INTERN_H

#include "Entity.h"
#include "Object.h"
#include "PathUtils.h"
#include <memory>
#include <stdexcept>

using EntityPtr = std::shared_ptr<Entity>;
using ConstEntityPtr = std::shared_ptr<const Entity>;

using EntityHashType = std::uint32_t;

class EntityPool {
public:
    static std::shared_ptr<EntityPool> getInstance() {
        static std::shared_ptr<EntityPool> instance(new EntityPool());
        return instance;
    }

    EntityPtr try_emplace(const std::string& name, const std::string& parent_path) {
        auto entity_path = parent_path.empty() ? name : parent_path + "/" + name;
        //return EntityPool::getInstance()->intern(name, fnv1a(entity_path.c_str(), entity_path.length()));
        return intern(name, fnv1a(entity_path.c_str(), entity_path.length()));
    }

    EntityPtr intern(const std::string& str, std::uint32_t hash);

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
    EntityPool() = default;

    void clearPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!pool_.empty()) {
            auto it = pool_.begin();
            pool_.erase(it); // Erase from the pool map
        }
    }

    std::mutex mutex_;
    std::unordered_map<std::uint32_t, std::weak_ptr<const Entity>> pool_;
};

class EntityRef {
public:

    static EntityRef make(const std::string& full_path) {
        auto leaf_name = PathUtils::get_leafname(full_path);
        auto parent_name = PathUtils::get_parent(full_path);
        return EntityRef(leaf_name, parent_name);
    }

    EntityRef(const std::string& name, const std::string& parent_path = "")
        : ptr_(EntityPool::getInstance()->try_emplace(name, parent_path))
    {}

    EntityRef(const std::string& name, EntityRef parent)
        : EntityRef(name, parent.getPath())
    {}
    EntityRef(EntityPtr ptr)
        : EntityRef(ptr->getName(), ptr->getParent()->getPath())
    {}

    EntityRef() : ptr_(nullptr) {}
    EntityRef(ConstEntityPtr ptr) : ptr_(std::const_pointer_cast<Entity>(ptr)) {}

    //EntityRef(EntityPtr& ptr) : ptr_(ptr) {}

    EntityRef(const EntityRef& other) : ptr_(other.ptr_) {}
    EntityRef& operator=(const EntityRef& other) {
        ptr_ = other.ptr_;
        return *this;
    }

    EntityRef(EntityRef&& other) noexcept : ptr_(std::move(other.ptr_)) {}
    EntityRef& operator=(EntityRef&& other) noexcept {
        if (this != &other) {
            ptr_ = std::move(other.ptr_);
        }
        return *this;
    }

    // Explicit conversion to std::shared_ptr<const Entity>
    explicit operator ConstEntityPtr() const {
        return std::const_pointer_cast<const Entity>(ptr_);
    }

    explicit operator bool() const {
        return isValid();
    }

    bool operator !() const {
        return not isValid();
    }

    const Entity* operator->() const {
        return ptr_.get();
    }

    const Entity& operator*() const {
        return *ptr_;
    }

    bool operator==(const EntityRef& other) const {
        return ptr_.get() == other.ptr_.get();
    }

    bool operator!=(const EntityRef& other) const {
        return ptr_.get() != other.ptr_.get();
    }

    const Entity* getRawPointer() const {
        return ptr_.get();
    }

    const std::string& getName() const {
        return ptr_->getName();
    }

    bool isValid() const {
        if (ptr_ != nullptr) return true;
        else return false;
    }

    void addChild(EntityRef child) {
        ptr_->addChild(std::const_pointer_cast<Entity>(child.ptr_));
    }

    EntityRef getChild(const std::string& name) const {
        return EntityRef(ptr_->getChild(name));
    }

    void setParent(EntityRef parent) {
        ptr_->setParent(parent.ptr_);
    }

    std::string getPath() const {
        return ptr_->getPath();
    }

    EntityRef getParent() const {
        return EntityRef(ptr_->getParent()); 
    }

    std::unordered_map<StringRef, std::shared_ptr<void>> getObjects() const {
        return ptr_->getObjects();
    }

    template <typename T>
    void setObject(const std::shared_ptr<T>& object) {
        ptr_->setObject<T>(object);
    }

    template <typename T>
    auto getObject() const {
        return std::move(ptr_->template getObject<T>());
    }

    void setObject(StringRef type_name, std::shared_ptr<void> object) {
        ptr_->setObject(type_name, object);
    }

    std::shared_ptr<void> getObject(StringRef type_name) const {
        return ptr_->getObject(type_name);
    }

    template<class T>
    ObjPtr<T> make_object(std::optional<GenericRef> rfl_generic = std::nullopt) {
        if (!isValid()) {
            throw std::runtime_error("entity is invalid");
            //return Error("entity is invalid.");
        }
        if constexpr(has_generic_v<T>) {
            using GenericType = typename T::GenericType;
            using GenericObj = typename std::pointer_traits<GenericType>::element_type;
            auto generate_ptr =getOrCreateObject<GenericObj>(std::cref(rfl_generic));
            auto ptr = getOrCreateObject<T>(generate_ptr);
            return ObjPtr<T>::make(std::move(ptr));
        } else {
            auto ptr = getOrCreateObject<T>(rfl_generic);
            return ObjPtr<T>::make(std::move(ptr));
        }
    }

    //// must constexpr(has_generic_v<T>
 //}


    //template <typename T, typename E, typename... Args>
    //std::shared_ptr<T> getOrCreateObject(Args&&... args) {
        //ptr_->getOrCreateObject<T, E>(std::forward<Args>(...args));
    //}

    template <typename T>
    auto getOrCreateObject(std::optional<GenericRef> rfl_generic) const {
        static_assert(!has_generic_v<T>, "must !has_generic_v class call with rfl GenericRef as argmument");
        return ptr_->getOrCreateObject<T>(rfl_generic);
    }

    template <typename T, typename ObjType>
    auto getOrCreateObject(std::shared_ptr<ObjType> generic_obj) const {
        static_assert(has_generic_v<T>, "must has_generic_v class call with GenericType as argmument");
        return ptr_->getOrCreateObject<T, ObjType>(generic_obj);
    }

    const auto& getChildren() const {
        return ptr_->getChildren();
    }

    const auto& getProperties() const {
        return ptr_->getProperties();
    }

    ValueType getProperty(const std::string& type_name, const std::string& member_name) const {
        return ptr_->getProperty(type_name, member_name);
    }

    void setProperty(StringRef type_name, const std::string& member_name, ValueType value) {
        ptr_->setProperty(type_name, member_name, value);
    }

    void setSerialize(StringRef type_name, const std::string& value_str) {
        ptr_->setSerialize(type_name, value_str);
    }

    void setSerialize(StringRef type_name, ValueType value) {
        ptr_->setSerialize(type_name, value);
    }

    void deserialize(StringRef type_name) {
        ptr_->deserialize(type_name);
    }

    rfl::Generic getSerialize(StringRef type_name) {
        return ptr_->getSerialize(type_name);
    }

    const auto& getSerializies() const {
        return ptr_->getSerializies();
    }

    void accept(Visitor<void>& visitor, int level = 0) const {
        ptr_->accept(visitor, level);
    }

    EntityRef findEntity(const std::string& path) const {
        return EntityRef(ptr_->findEntity(path));
    }

    bool isSelfObject() const {
        return ptr_->isSelfObject();
    }

    StringRef getSelfTypeName() const { 
        return ptr_->getSelfTypeName();
    }

    void setSelfTypeName(StringRef type_name) {
        ptr_->setSelfTypeName(type_name);
    }

    std::uint32_t getHash() const {
        return ptr_->getHash();
    }

    static EntityRef getEntityByHash(EntityHashType hash) {
        return EntityRef(EntityPool::getInstance()->getEntityByHash(hash));
    }

private:
    EntityPtr ptr_;
};

inline EntityPtr EntityPool::intern(const std::string& str, std::uint32_t hash) {
    std::lock_guard<std::mutex> lock(mutex_);


    auto it = pool_.find(hash);
    if (it != pool_.end()) {
        auto locked = it->second.lock();
        if (locked) {
            return std::const_pointer_cast<Entity>(locked);
        }
        return std::shared_ptr<Entity>();
    }
    auto deleter = [this, hash](const Entity* p) {
        std::lock_guard<std::mutex> lock(this->mutex_);
        this->pool_.erase(hash);
        delete p;
    };
    auto entity_ref = EntityPtr(new Entity(str, hash), deleter);
    pool_.emplace(hash, std::weak_ptr<Entity>(entity_ref));
    return entity_ref;
}

struct EntityGeneric {
  std::string entityPath;
  std::string entitytype;

  static EntityGeneric from_class(const EntityRef& _p) noexcept {
      return EntityGeneric{.entityPath = _p.getPath(),
                           .entitytype = _p.getSelfTypeName()->str()};
  }
};

namespace rfl {
namespace parsing {

template <class ReaderType, class WriterType, class ProcessorsType>
struct Parser<ReaderType, WriterType, EntityRef,
              ProcessorsType>
    : public CustomParser<ReaderType, WriterType, ProcessorsType,
                          EntityRef,
                          EntityGeneric> {};

}  // namespace parsing
}  // namespace rfl



#endif // STRING_INTERN_H
