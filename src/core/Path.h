#ifndef PATH_H
#define PATH_H

#include "Entity.h"
#include "Object.h"
#include "PathUtils.h"
#include <memory>
#include <stdexcept>
#include <mutex>

using EntityPtr = std::shared_ptr<Entity>;
using ConstEntityPtr = std::shared_ptr<const Entity>;

using EntityHashType = std::uint32_t;

class PathPool {
public:
    static std::shared_ptr<PathPool> getInstance() {
        static std::shared_ptr<PathPool> instance(new PathPool());
        return instance;
    }

    EntityPtr tryEmplace(const std::string& name, const std::string& parent_path) {
        auto entity_path = parent_path.empty() ? name : parent_path + "/" + name;
        return intern(name, fnv1a(entity_path.c_str(), entity_path.length()));
    }

    EntityPtr intern(const std::string& str, std::uint32_t hash);

    ~PathPool() {
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
    PathPool() = default;

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

class Path {
public:
    static Path make(const std::string& full_path) {
        auto leaf_name = PathUtils::getLeafName(full_path);
        auto parent_name = PathUtils::getParent(full_path);
        return Path(leaf_name, parent_name);
    }

    static Path make(const std::string& leaf_name, const std::string& parent_name) {
        return Path(leaf_name, parent_name);
    }

    Path(const std::string& name, const std::string& parent_path = "")
        : entity_ptr_(PathPool::getInstance()->tryEmplace(name, parent_path))
    {}

    Path(const std::string& name, Path parent)
        : Path(name, parent.getPath())
    {
        if (parent.isObjectPath()) {
            object_parts_.push_back(name);
        } else {
            auto entity_name = name;
            auto pos = entity_name.find(':');
            bool find_object = false;
            if (pos != std::string::npos) {
                auto type_name = PathUtils::parseTypeName(name.substr(pos + 1));
                object_type_ = type_name;

                entity_name = entity_name.substr(0, pos);

            }

            entity_parts_.push_back(entity_name);

            auto child_entity_ptr = parent.getChildEntity(entity_name);
            entity_ptr_ = child_entity_ptr;

        }
    }

    Path(EntityPtr ptr)
        : entity_ptr_(ptr ? (PathPool::getInstance()->tryEmplace(ptr->getName(), ptr->getParent()->getPath()))
                   : nullptr)
    {}

    Path() : entity_ptr_(nullptr) {}
    Path(ConstEntityPtr ptr) : entity_ptr_(std::const_pointer_cast<Entity>(ptr)) {}

    Path(const Path& other) : entity_ptr_(other.entity_ptr_) {}
    Path& operator=(const Path& other) {
        entity_ptr_ = other.entity_ptr_;
        return *this;
    }

    Path(Path&& other) noexcept : entity_ptr_(std::move(other.entity_ptr_)) {}
    Path& operator=(Path&& other) noexcept {
        if (this != &other) {
            entity_ptr_ = std::move(other.entity_ptr_);
        }
        return *this;
    }

    explicit operator ConstEntityPtr() const {
        return std::const_pointer_cast<const Entity>(entity_ptr_);
    }

    explicit operator bool() const {
        return isValid();
    }

    bool operator!() const {
        return !isValid();
    }

    const Entity* operator->() const {
        return entity_ptr_.get();
    }

    const Entity& operator*() const {
        return *entity_ptr_;
    }

    bool operator==(const Path& other) const {
        return entity_ptr_.get() == other.entity_ptr_.get();
    }

    bool operator!=(const Path& other) const {
        return entity_ptr_.get() != other.entity_ptr_.get();
    }

    const Entity* getRawPointer() const {
        return entity_ptr_.get();
    }

    const std::string& getName() const {
        return entity_ptr_->getName();
    }

    bool isValid() const {
        return entity_ptr_ != nullptr;
    }

    void addChild(Path child) {
        entity_ptr_->addChild(std::const_pointer_cast<Entity>(child.entity_ptr_));
    }

    EntityPtr getChildEntity(const std::string& name) const {
        auto entity_ptr = entity_ptr_->getChild(name);
        return entity_ptr;
    }

    void setParent(Path parent) {
        entity_ptr_->setParent(parent.entity_ptr_);
    }

    std::string getPath() const {
        return entity_ptr_->getPath();
    }

    Path getParent() const {
        return Path(entity_ptr_->getParent());
    }

    auto getObjects() const {
        return entity_ptr_->getObjects();
    }

    template <typename T>
    void setObject(const std::shared_ptr<T>& object) {
        entity_ptr_->setObject<T>(object);
    }

    template <typename T>
    auto getObject() const {
        return entity_ptr_->template getObject<T>();
    }

    void setObject(StringRef type_name, std::shared_ptr<void> object) {
        entity_ptr_->setObject(type_name, object);
    }

    std::shared_ptr<void> getObject(StringRef type_name) const {
        return entity_ptr_->getObject(type_name);
    }

    template <class T>
    ObjPtr<T> make_object(std::optional<GenericRef> rfl_generic = std::nullopt) {
        if (!isValid()) {
            throw std::runtime_error("entity is invalid");
        }
        if constexpr(has_generic_v<T>) {
            using GenericType = typename T::GenericType;
            using GenericObj = typename std::pointer_traits<GenericType>::element_type;
            auto generate_ptr = entity_ptr_->getOrCreateObject<GenericObj>(std::cref(rfl_generic));
            auto ptr = entity_ptr_->getOrCreateObject<T>(generate_ptr);
            return ObjPtr<T>::make(std::move(ptr));
        } else {
            auto ptr = entity_ptr_->getOrCreateObject<T>(rfl_generic);
            return ObjPtr<T>::make(std::move(ptr));
        }
    }

    const auto& getChildren() const {
        return entity_ptr_->getChildren();
    }

    const auto& getProperties() const {
        return entity_ptr_->getProperties();
    }

    ValueType getProperty(const std::string& type_name, const std::string& member_name) const {
        return entity_ptr_->getProperty(type_name, member_name);
    }

    void setProperty(StringRef type_name, const std::string& member_name, ValueType value) {
        assert(object_type_ == "");
        entity_ptr_->setProperty(type_name, member_name, value);
    }

    void setProperty(const std::string& member_name, ValueType value) {
        assert(object_type_ != "");
        entity_ptr_->setProperty(object_type_, member_name, value);
    }

    void setSerialize(StringRef type_name, const std::string& value_str) {
        entity_ptr_->setSerialize(type_name, value_str);
    }

    void setSerialize(ValueType value) {
        assert(object_type_ != "");
        entity_ptr_->setSerialize(object_type_, value);
    }

    void deserialize() const {
        assert(object_type_ != "");
        entity_ptr_->deserialize(object_type_);
    }

    rfl::Generic getSerialize(StringRef type_name) const {
        return entity_ptr_->getSerialize(type_name);
    }

    const auto& getSerializies() const {
        return entity_ptr_->getSerializies();
    }

    void accept(Visitor<void>& visitor, int level = 0) const {
        entity_ptr_->accept(visitor, level);
    }

    Path findEntity(const std::string& path) const {
        return Path(entity_ptr_->findEntity(path));
    }

    bool isObjectPath() const {
        return object_type_ != "";
    }

    StringRef getTypeName() const {
        return object_type_;
    }

    void setTypeName(StringRef type_name) {
        object_type_ = type_name;
    }

    auto getObjectPath() {
        return object_parts_;
    }
    void pushObjectPath(StringRef object_leafname) {
        object_parts_.push_back(object_leafname);
    }

    std::uint32_t getHash() const {
        return entity_ptr_->getHash();
    }

    static Path getEntityByHash(EntityHashType hash) {
        return Path(PathPool::getInstance()->getEntityByHash(hash));
    }

private:
    EntityPtr entity_ptr_;
    StringRef object_type_;
    std::vector<StringRef> object_parts_;
    std::vector<StringRef> entity_parts_;
};

inline EntityPtr PathPool::intern(const std::string& str, std::uint32_t hash) {
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
    std::string entityType;

    static EntityGeneric from_class(const Path& _p) noexcept {
        return EntityGeneric{.entityPath = _p.getPath(),
                            .entityType = _p.getTypeName()->str()};
    }
};

namespace rfl {
namespace parsing {

template <class ReaderType, class WriterType, class ProcessorsType>
struct Parser<ReaderType, WriterType, Path, ProcessorsType>
    : public CustomParser<ReaderType, WriterType, ProcessorsType, Path, EntityGeneric> {};

}  // namespace parsing
}  // namespace rfl

#endif // PATH_H

