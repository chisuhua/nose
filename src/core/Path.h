#ifndef PATH_H
#define PATH_H

#include "EntityPool.h"
#include "ObjPtr.h"
#include "PathUtils.h"
#include "StringIntern.h"
#include <memory>
#include <stdexcept>


class Path {
public:
    static Path make(const std::string& full_path) {
        return Path(PathUtils::getEntityPath(full_path));
    }

    static Path make(const std::string& leaf_name, const std::string& parent_name) {
        auto full_path = PathUtils::join(parent_name, leaf_name);
        return Path(PathUtils::getEntityPath(full_path));
    }

    Path(const std::string& full_entity_path)
        : entity_ptr_(EntityPool::getInstance()->getEntity(full_entity_path))
    {}

    Path(const std::string& name, const std::string& parent_path)
        : entity_ptr_(EntityPool::getInstance()->getEntity(name, parent_path))
    {}

    Path(const std::string& name, const Path& current)
    {
        if (current.isObjectPath() ) {
            auto entity_name = name;
            auto pos = entity_name.find(':');
            bool find_object = false;
            if (pos != std::string::npos) {
                auto type_name = PathUtils::parseTypeName(name.substr(pos + 1));
                object_type_ = type_name;
                entity_name = entity_name.substr(0, pos);
                entity_ptr_ = current.entity_ptr_->getOrCreateChild(entity_name);
                object_parts_.clear();
            } else {
                entity_ptr_ = current.entity_ptr_;
                object_type_ = current.object_type_;
                for (size_t i = 0 ; i < current.object_parts_.size(); i++) {
                    if (entity_name != "..") {
                        object_parts_.push_back(current.object_parts_[i]);
                    } else if (i != current.object_parts_.size()) {
                        object_parts_.push_back(current.object_parts_[i]);
                    }
                }
                if (entity_name != "..") object_parts_.push_back(entity_name);

                if (object_parts_.size() == 0 and entity_name == "..") {
                    object_type_ = "";
                }
            }
        } else {
            if (name != "..") {
                auto entity_name = name;
                auto pos = entity_name.find(':');
                bool find_object = false;
                if (pos != std::string::npos) {
                    auto type_name = PathUtils::parseTypeName(name.substr(pos + 1));
                    object_type_ = type_name;

                    entity_name = entity_name.substr(0, pos);

                }

                //entity_parts_.clear();
                //for (auto i : current.entity_parts_) {
                    //entity_parts_.push_back(i);
                //}
                //entity_parts_.push_back(entity_name);

                auto child_entity_ptr = current.getChildEntity(entity_name);
                entity_ptr_ = child_entity_ptr;
            } else {
                auto parent_entity_ptr = current.entity_ptr_->getParent();
                entity_ptr_ = parent_entity_ptr;

                //entity_parts_.clear();
                //for (size_t i = 0;  i < (current.entity_parts_.size() -1) ; i++) {
                    //entity_parts_.push_back(current.entity_parts_[i]);
                //}

                //if (parent.isObjectPath() ) {
            }
        }
    }

    //Path(EntityPtr ptr)
        //: entity_ptr_(ptr ? (EntityPool::getInstance()->tryEmplace(ptr->getName(), ptr->getParent()->getPath()))
                   //: nullptr)
    //{}

    Path() : entity_ptr_(nullptr) {}
    Path(ConstEntityPtr ptr) : entity_ptr_(std::const_pointer_cast<Entity>(ptr)) {}

    Path(const Path& other) 
        : entity_ptr_(other.entity_ptr_) 
        , object_type_(other.object_type_) 
        , object_parts_(other.object_parts_) 
        //, entity_parts_(other.entity_parts_) 
    {}
    Path& operator=(const Path& other) {
        entity_ptr_ = other.entity_ptr_;
        object_type_ = other.object_type_;
        //entity_parts_.clear();
        //for (auto i : other.entity_parts_) {
            //entity_parts_.push_back(i);
        //}
        for (auto i : other.object_parts_) {
            object_parts_.push_back(i);
        }
        return *this;
    }

    Path(Path&& other) noexcept
        : entity_ptr_(std::move(other.entity_ptr_))
        , object_type_(std::move(other.object_type_))
        , object_parts_(std::move(other.object_parts_))
        //, entity_parts_(std::move(other.entity_parts_))
        {}

    Path& operator=(Path&& other) noexcept {
        if (this != &other) {
            entity_ptr_ = std::move(other.entity_ptr_);
            object_type_ = std::move(other.object_type_);
            //entity_parts_= std::move(other.entity_parts_);
            object_parts_= std::move(other.object_parts_);
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

    /// Returns the underlying shared_ptr
    EntityPtr& ptr() { return entity_ptr_; }

    /// Returns the underlying shared_ptr
    const EntityPtr& ptr() const { return entity_ptr_; }


    const Entity* getRawPointer() const {
        return entity_ptr_.get();
    }

    const std::string& getName() const {
        return entity_ptr_->getName();
    }

    bool isValid() const {
        return entity_ptr_ != nullptr;
    }

    void addChild(const Path& child) {
        entity_ptr_->addChild(std::const_pointer_cast<Entity>(child.entity_ptr_));
    }

    EntityPtr getChildEntity(const std::string& name) const {
        auto entity_ptr = entity_ptr_->getOrCreateChild(name);
        return entity_ptr;
    }

    void setParent(const Path& parent) {
        entity_ptr_->setParent(parent.entity_ptr_);
    }

    std::string getEntityPath() const {
        return entity_ptr_->getEntityPath();
    }

    Path getParent() const {
        return Path(entity_ptr_->getParent());
    }

    auto getObjects() const {
        return entity_ptr_->getObjects();
    }

    template <typename T>
    void setObject(ObjRef object) {
        entity_ptr_->setObject<T>(object);
    }

    template <typename T>
    ObjPtr<T> getObject() const {
        auto obj = entity_ptr_->template getObject<T>();
        return ObjPtr<T>::make(obj->template as<T>());
    }

    void setObject(StringRef type_name, ObjRef object) {
        entity_ptr_->setObject(type_name, object);
    }

    template <typename T>
    auto removeObject() {
        return entity_ptr_->removeObject<T>();
    }

    void removeObject(StringRef type_name) {
        entity_ptr_->removeObject(type_name);
    }

    ObjRef getObject(StringRef type_name) const {
        auto obj =  entity_ptr_->getObject(type_name);
        if (obj.has_value()) {
            return obj.value();
        } else {
            return ObjRef(""_hs, nullptr);
        }
    }

    template <class T>
    ObjPtr<T> make_object(std::optional<GenericRef> rfl_generic = std::nullopt) {
        if (!isValid()) {
            throw std::runtime_error("entity is invalid");
        }
        if constexpr(has_generic_v<T>) {
            using GenericType = ExtractedGenericType<T>;
            //using GenericObj = typename std::pointer_traits<GenericType>::element_type;
           
            //std::shared_ptr<GenericType> generic_ptr;
            //ObjRef generic_ptr;
            if (rfl_generic) {
                auto generic_ptr = entity_ptr_->getOrCreateObject<GenericType>(std::cref(rfl_generic.value()));
                auto ptr = entity_ptr_->getOrCreateObject<T>(std::move(generic_ptr.template as<GenericType>()));
                return ObjPtr<T>::make(std::move(ptr.template as<T>()));
            } else {
                auto generic_ptr = entity_ptr_->getOrCreateObject<GenericType>();
                auto ptr = entity_ptr_->getOrCreateObject<T>(generic_ptr.template as<GenericType>());
                return ObjPtr<T>::make(std::move(ptr.template as<T>()));
            }
        } else {
            std::shared_ptr<T> ptr;
            if (rfl_generic) {
                ptr = entity_ptr_->getOrCreateObject<T>(rfl_generic.value()).template as<T>();
            } else {
                ptr = entity_ptr_->getOrCreateObject<T>().template as<T>();
            }
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

    void setSerialize(const std::string& value_str) {
        assert(object_type_ != "");
        entity_ptr_->setSerialize(object_type_, value_str);
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

    template<typename T = void>
    void accept(Visitor<T>& visitor, int level = 0) const {
        entity_ptr_->accept(visitor, level);
    }

    Path findEntity(const std::string& path) const {
        return Path(entity_ptr_->findEntity(path));
    }

    bool isObjectPath() const {
        return not !object_type_;
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
        return Path(EntityPool::getInstance()->getEntityByHash(hash));
    }

private:
    EntityPtr entity_ptr_;
    StringRef object_type_;
    std::vector<StringRef> object_parts_;
    //std::vector<StringRef> entity_parts_;
};


struct EntityGeneric {
    std::string entityPath;
    std::string entityType;

    static EntityGeneric from_class(const Path& _p) noexcept {
        return EntityGeneric{.entityPath = _p->getEntityPath(),
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

