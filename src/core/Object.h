#ifndef OBJECT_H
#define OBJECT_H
#include "TypeInfo.h"
#include <memory>

class ObjRef {
public:
    explicit ObjRef(StringRef type_name, VoidPtrType ptr) 
        :type_name_(type_name)
        ,ptr_(ptr) {};

    ObjRef(const ObjRef& _other) = default;
    ObjRef& operator=(const ObjRef& other) = default;

    ObjRef(ObjRef&& other) noexcept
        : type_name_(std::move(other.type_name_))
        , ptr_(std::move(other.ptr_)) 
        {}

    ObjRef& operator=(ObjRef&& other) noexcept {
        if (this != &other) {
            type_name_ = std::move(other.type_name_);
            ptr_ = std::move(other.ptr_);
            return *this;
        }
    }

    VoidPtrType& ptr() { return ptr_; };
    const VoidPtrType& ptr() const { return ptr_; };

    template<typename T>
    std::shared_ptr<T> as() {
        if (TypeInfo::getTypeName<T>() == type_name_) {
            return std::static_pointer_cast<T>(ptr_);
        } else {
            return nullptr;
        }
    }

    template<typename T>
    const std::shared_ptr<T> as() const {
        if (TypeInfo::getTypeName<T>() == type_name_) {
            return std::static_pointer_cast<T>(ptr_);
        } else {
            return nullptr;
        }
    }

    StringRef type_name_;
    VoidPtrType ptr_;
};
#endif
