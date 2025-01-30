#ifndef OBJECT_H
#define OBJECT_H

#include "rfl/Result.hpp"
//#include <refl.hpp>
//#include "EntityIntern.h"
#include "TypeInfo.h"
#include <stdexcept>
#include <type_traits>
#include <memory>


/*
EntityRef represent as path: /a/b, /a/b/c, /a/b/c/d
ObjPath represent as path:Type, like /a/b:Type1, /a/b:Type2
/a/b:Type1
/a/b:Type2
/a/b/c:Type1
/a/b/c:Type2
/a/b/c/d:Type1
*/

//struct ObjectRefSerialize {

    //static ObjectRefSerialize from_class(const ObjectRef& _p) noexcept {
        //return ObjectRefSerialize{.entity_path_ = _p.getEntityPath(),
                                  //.type_name_ = _p.getTypeName()};
    //}
//};


using namespace rfl;

template <class T>
class ObjPath {
    //std::string entity_path_;
    //std::string type_name_;
    StringRef entity_path_;
    StringRef type_name_;
};

/// The ObjPtr class behaves very similarly to the shared_ptr, but unlike the
/// unique_ptr, it is 100% guaranteed to be filled at all times (unless the user
/// tries to access it after calling std::move does something else that is
/// clearly bad practice).
template <class T, typename=void>
class ObjPtr;

template <class T>
class ObjPtr<T, std::enable_if_t<!has_generic_v<T>>> {
 public:
  /// The default way of creating new references is
  /// ObjPtr<T>::make(...) or make_ref<T>(...).
  template <class... Args>
  static ObjPtr<T> make(Args&&... _args) {
    return ObjPtr<T>(std::make_shared<T>(std::forward<Args>(_args)...));
  }

  //static ObjPtr<T> make(EntityRef entity, std::optional<GenericRef> rfl_generic = std::nullopt) {
    //if (!entity.isValid()) {
      //return Error("entity is invalid.");
    //}

    //auto ptr = entity.getOrCreateObject<T>(rfl_generic);
    //return ObjPtr<T>(std::move(ptr));
  //}


  /// You can generate them from shared_ptrs as well, in which case it will
  /// return an Error, if the shared_ptr is not set.
  //static Result<ObjPtr<T>> make(ExtractedGenericType<T>&& _ptr) ;
  static ObjPtr<T> make(T::GenericType&& _ptr) {
    if (!_ptr) {
      return Error("std::shared_ptr was a nullptr.");
    }
    return ObjPtr<T>(std::move(_ptr));
  }


  /// You can generate them from shared_ptrs as well, in which case it will
  /// return an Error, if the shared_ptr is not set.
  static ObjPtr<T> make(std::shared_ptr<T>&& _ptr) {
    if (!_ptr) {
      return Error("std::shared_ptr was a nullptr.");
    }
    return ObjPtr<T>(std::move(_ptr));
  }

  /// You can generate them from shared_ptrs as well, in which case it will
  /// return an Error, if the shared_ptr is not set.
  static ObjPtr<T> make(const std::shared_ptr<T>& _ptr) {
    if (!_ptr) {
      return Error("std::shared_ptr was a nullptr.");
    }
    return ObjPtr<T>(_ptr);
  }

  ObjPtr() : ptr_(std::make_shared<T>()) {}

  ObjPtr(const ObjPtr<T>& _other) = default;

  ObjPtr(ObjPtr<T>&& _other) = default;

  template <class U>
  ObjPtr(const ObjPtr<U>& _other) : ptr_(_other.ptr()) {}

  template <class U>
  ObjPtr(ObjPtr<U>&& _other) noexcept
      : ptr_(std::forward<std::shared_ptr<U>>(_other.ptr())) {}

  ~ObjPtr() = default;

  /// Returns a pointer to the underlying object
  T* get() const { return ptr_.get(); }

  /// Returns the underlying object.
  T& operator*() { return *ptr_; }

  /// Returns the underlying object.
  T& operator*() const { return *ptr_; }

  /// Returns the underlying object.
  T* operator->() { return ptr_.get(); }

  /// Returns the underlying object.
  T* operator->() const { return ptr_.get(); }

  /// Returns the underlying shared_ptr
  std::shared_ptr<T>& ptr() { return ptr_; }

  /// Returns the underlying shared_ptr
  const std::shared_ptr<T>& ptr() const { return ptr_; }

  /// Copy assignment operator.
  template <class U>
  ObjPtr<T>& operator=(const ObjPtr<U>& _other) {
    ptr_ = _other.ptr();
    return *this;
  }

  /// Move assignment operator
  template <class U>
  ObjPtr<T>& operator=(ObjPtr<U>&& _other) noexcept {
    ptr_ = std::forward<std::shared_ptr<U>>(_other.ptr());
    return *this;
  }

  /// Move assignment operator
  ObjPtr<T>& operator=(ObjPtr<T>&& _other) noexcept = default;

  /// Copy assignment operator
  ObjPtr<T>& operator=(const ObjPtr<T>& _other) = default;

 private:
  /// Only make is allowed to use this constructor.
  explicit ObjPtr(std::shared_ptr<T>&& _ptr) : ptr_(std::move(_ptr)) {}

  /// Only make is allowed to use this constructor.
  explicit ObjPtr(const std::shared_ptr<T>& _ptr) : ptr_(_ptr) {}

 private:
  /// The underlying shared_ptr_
  std::shared_ptr<T> ptr_;
};

template <class T>
class ObjPtr<T, std::enable_if_t<has_generic_v<T>>> {
 public:
  /// The default way of creating new references is
  /// ObjPtr<T>::make(...) or make_ref<T>(...).
  template <class... Args>
  static ObjPtr<T> make(Args&&... _args) {
    return ObjPtr<T>(std::make_shared<T>(std::forward<Args>(_args)...));
  }

  //static Result<ObjPtr<T>> make(EntityRef entity, std::optional<GenericRef> rfl_generic = std::nullopt) {
    //if (!entity.isValid()) {
      //return Error("entity is invalid.");
    //}

    //// must constexpr(has_generic_v<T>
    //using GenericType = typename T::GenericType;
    //using GenericObj = typename std::pointer_traits<GenericType>::element_type;
    //auto generate_ptr = entity.getOrCreateObject<GenericObj>(std::cref(rfl_generic));
    //auto ptr = entity.getOrCreateObject<T>(generate_ptr);
    //return ObjPtr<T>(std::move(ptr));
  //}



  /// You can generate them from shared_ptrs as well, in which case it will
  /// return an Error, if the shared_ptr is not set.
  //static Result<ObjPtr<T>> make(ExtractedGenericType<T>&& _ptr) ;
  static ObjPtr<T> make(T::GenericType&& _ptr) {
    if (!_ptr) {
        throw std::runtime_error("std::shred_ptr<T> was a nullptr");
      //return Error("std::shared_ptr was a nullptr.");
    }
    return ObjPtr<T>(std::move(_ptr));
  }


  /// You can generate them from shared_ptrs as well, in which case it will
  /// return an Error, if the shared_ptr is not set.
  static ObjPtr<T> make(std::shared_ptr<T>&& _ptr) {
    if (!_ptr) {
        throw std::runtime_error("std::shred_ptr<T> was a nullptr");
      //return Error("std::shared_ptr was a nullptr.");
    }
    return ObjPtr<T>(std::move(_ptr));
  }

  /// You can generate them from shared_ptrs as well, in which case it will
  /// return an Error, if the shared_ptr is not set.
  static ObjPtr<T> make(const std::shared_ptr<T>& _ptr) {
    if (!_ptr) {
        throw std::runtime_error("std::shred_ptr<T> was a nullptr");
      //return Error("std::shared_ptr was a nullptr.");
    }
    return ObjPtr<T>(_ptr);
  }

  ObjPtr() : ptr_(std::make_shared<T>()) {}

  ObjPtr(const ObjPtr<T>& _other) = default;

  ObjPtr(ObjPtr<T>&& _other) = default;

  template <class U>
  ObjPtr(const ObjPtr<U>& _other) : ptr_(_other.ptr()) {}

  template <class U>
  ObjPtr(ObjPtr<U>&& _other) noexcept
      : ptr_(std::forward<std::shared_ptr<U>>(_other.ptr())) {}

  ~ObjPtr() = default;

  /// Returns a pointer to the underlying object
  T* get() const { return ptr_.get(); }

  /// Returns the underlying object.
  T& operator*() { return *ptr_; }

  /// Returns the underlying object.
  T& operator*() const { return *ptr_; }

  /// Returns the underlying object.
  T* operator->() { return ptr_.get(); }

  /// Returns the underlying object.
  T* operator->() const { return ptr_.get(); }

  /// Returns the underlying shared_ptr
  std::shared_ptr<T>& ptr() { return ptr_; }

  /// Returns the underlying shared_ptr
  const std::shared_ptr<T>& ptr() const { return ptr_; }

  /// Copy assignment operator.
  template <class U>
  ObjPtr<T>& operator=(const ObjPtr<U>& _other) {
    ptr_ = _other.ptr();
    return *this;
  }

  /// Move assignment operator
  template <class U>
  ObjPtr<T>& operator=(ObjPtr<U>&& _other) noexcept {
    ptr_ = std::forward<std::shared_ptr<U>>(_other.ptr());
    return *this;
  }

  /// Move assignment operator
  ObjPtr<T>& operator=(ObjPtr<T>&& _other) noexcept = default;

  /// Copy assignment operator
  ObjPtr<T>& operator=(const ObjPtr<T>& _other) = default;

 private:
  /// Only make is allowed to use this constructor.
  explicit ObjPtr(std::shared_ptr<T>&& _ptr) : ptr_(std::move(_ptr)) {}

  /// Only make is allowed to use this constructor.
  explicit ObjPtr(const std::shared_ptr<T>& _ptr) : ptr_(_ptr) {}

 private:
  /// The underlying shared_ptr_
  std::shared_ptr<T> ptr_;
};

/// Generates a new ObjPtr<T>.
template <class T, class... Args>
auto make_object(Args&&... _args) {
  return ObjPtr<T>::make(std::forward<Args>(_args)...);
}

template <class T1, class T2>
inline auto operator<=>(const ObjPtr<T1>& _t1, const ObjPtr<T2>& _t2) {
  return _t1.ptr() <=> _t2.ptr();
}

template <class CharT, class Traits, class T>
inline std::basic_ostream<CharT, Traits>& operator<<(
    std::basic_ostream<CharT, Traits>& _os, const ObjPtr<T>& _b) {
  _os << _b.get();
  return _os;
}

namespace std {

template <class T>
struct hash<ObjPtr<T>> {
  size_t operator()(const ObjPtr<T>& _r) const {
    return hash<shared_ptr<T>>()(_r.ptr());
  }
};

template <class T>
inline void swap(ObjPtr<T>& _r1, ObjPtr<T>& _r2) {
  return swap(_r1.ptr(), _r2.ptr());
}

}  // namespace std

#include "ObjectRflParser.h"


#endif
