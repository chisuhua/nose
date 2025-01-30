#ifndef OBJECT_RFL_PARSER_HPP_
#define OBJECT_RFL_PARSER_HPP_

#include <map>
#include <type_traits>

//#include "Object.h"
#include "rfl/Result.hpp"
//#include "rfl/always_false.hpp"
#include "rfl/parsing/Parser_base.hpp"
#include "rfl/parsing/schema/Type.hpp"

namespace rfl {
namespace parsing {

//template <class R, class W, class T, class ProcessorsType>
//requires AreReaderAndWriter<R, W, ObjPtr<T>>
//struct Parser<R, W, ObjPtr<T>, ProcessorsType> {
  //using InputVarType = typename R::InputVarType;

  //static Result<ObjPtr<T>> read(const R& _r, const InputVarType& _var) noexcept {
    //const auto to_ref = [&](auto&& _t) { return ObjPtr<T>::make(std::move(_t)); };
    //return Parser<R, W, std::remove_cvref_t<T>, ProcessorsType>::read(_r, _var)
        //.transform(to_ref);
  //}

  //template <class P>
  //static void write(const W& _w, const ObjPtr<T>& _ref,
                    //const P& _parent) noexcept {
    //Parser<R, W, std::remove_cvref_t<T>, ProcessorsType>::write(_w, *_ref,
                                                                //_parent);
  //}

  //static schema::Type to_schema(
      //std::map<std::string, schema::Type>* _definitions) {
    //return Parser<R, W, std::remove_cvref_t<T>, ProcessorsType>::to_schema(
        //_definitions);
  //}
//};

template <class R, class W, class ProcessorsType, class ObjPtrClass,
          class HelperStruct>
struct ObjPtrParser {
  static Result<ObjPtrClass> read(const R& _r, const auto& _var) noexcept {
    const auto to_class = [](auto&& _h) -> Result<ObjPtrClass> {
      try {
          //auto ptr_field_tuple = internal::to_ptr_field_tuple(*_h);
          //const auto class_from_ptrs = [](auto&... _ptrs) {
            //return ObjPtrClass(std::move(*_ptrs.value_)...);
          //};
          //return rfl:apply(class_from_ptrs, ptr_field_tuple);
          return ObjPtrClass::make(_h);
      } catch (std::exception& e) {
        return Error(e.what());
      }
    };
    return Parser<R, W, HelperStruct, ProcessorsType>::read(_r, _var).and_then(
        to_class);
  }

  template <class P>
  static auto write(const W& _w, const ObjPtrClass& _p,
                    const P& _parent) noexcept {
    Parser<R, W, HelperStruct, ProcessorsType>::write(
        _w, _p->generic_, _parent);
        //_w, HelperStruct::from_class(_p), _parent);
  }

  static schema::Type to_schema(
      std::map<std::string, schema::Type>* _definitions) {
    return Parser<R, W, std::remove_cvref_t<HelperStruct>,
                  ProcessorsType>::to_schema(_definitions);
  }
};

template <class R, class W, class T, class ProcessorsType>
requires AreReaderAndWriter<R, W, ObjPtr<T>>
struct Parser<R, W, ObjPtr<T>, ProcessorsType>
    : public ObjPtrParser<R, W, ProcessorsType,
                          ObjPtr<T>,
                          typename T::GenericType> {};


}  // namespace parsing
}  // namespace rfl

#endif
