#pragma once
#include <refl.hpp>
#include <Registry.h>
#include <type_traits>

template<typename Type>
struct in_place_delete {
    static constexpr bool value = !std::is_move_constructible<Type>::value && !std::is_move_assignable<Type>::value;
};

template<>
struct in_place_delete<void> : std::false_type {};

class EntityNull {
public:
    using ObjectId = uint32_t;
    using EntityId = uint32_t;
};

//class Object {
//public: 
    //StringRef type_name;
    //std::shared_ptr<void> ptr_; // ptr_ from createObjectByName
//};
using ObjectPtr = std::shared_ptr<void>;

class ObjectRef {
public:
    ObjectRef(ObjectPtr ptr) : ptr_(ptr) {};
    ObjectRef(ObjectPtr& ptr) : ptr_(ptr) {};
    ObjectRef(const ObjectRef& other) : ptr_(other.ptr_) {};

    ObjectRef(ObjectRef&& other) noexcept : ptr_(std::move(other.ptr_)) {}
    ObjectRef& operator=(ObjectRef&& other) noexcept {
        if (this != &other) {
            ptr_ = std::move(other.ptr_);
        }
        return *this;
    }

    // 获取共享指针
    operator ObjectPtr() const {
        return ptr_;
    }

    template<typename T>
    std::shared_ptr<T>* as() {
        std::static_pointer_cast<T>(ptr_);
    }

    //const Object* operator->() const {
        //return ptr_.get();
    //}

    //const Object& operator*() const {
        //return *ptr_;
    //}

    bool operator==(const ObjectRef& other) const {
        return ptr_.get() == other.ptr_.get();
    }

    bool operator!=(const ObjectRef& other) const {
        return ptr_.get() != other.ptr_.get();
        //return !(*this == other);
    }

    //const Object* getRawPointer() const {
        //return ptr_.get();
    //}

private:
    ObjectPtr ptr_;
};


