#ifndef TYPEINFO_H
#define TYPEINFO_H
#include <typeinfo>
#include <cxxabi.h>
#include "StringIntern.h"

template<typename Type>
struct in_place_delete {
    static constexpr bool value = !std::is_move_constructible<Type>::value && !std::is_move_assignable<Type>::value;
};

template<>
struct in_place_delete<void> : std::false_type {};

// Primary template: if the type T does not have a member type SubType, this will be used.
template<typename T, typename = void>
struct has_generic_t : std::false_type {};

// Specialization that will be selected when T has a member type SubType.
template<typename T>
struct has_generic_t<T, std::void_t<typename T::GenericType>> : std::true_type {};

// Helper variable template for easier use
template<typename T>
inline constexpr bool has_generic_v = has_generic_t<T>::value;

template<typename T, typename = void>
struct has_owner_t : std::false_type {};

// Specialization that will be selected when T has a member type SubType.
template<typename T>
struct has_owner_t<T, std::void_t<typename T::OwnerType>> : std::true_type {};

// Helper variable template for easier use
template<typename T>
inline constexpr bool has_owner_v = has_owner_t<T>::value;

// 提取 GenericType 的结构模板
template<typename T, typename = void>
struct ExtractGenericType {
    // 默认情况下不做任何事情
};

// 特化版本：当 T 有 GenericType 成员类型时
template<typename T>
struct ExtractGenericType<T, std::enable_if_t<has_generic_v<T>>> {
    using type = typename T::GenericType::element_type;
};

// 提取类型别名的便捷定义
template<typename T>
using ExtractedGenericType = typename ExtractGenericType<T>::type;


class TypeInfo
{
public:

    // instances can be obtained only through calls to Get()
    template <typename T>
    static const TypeInfo& Get()
    {
        // here we create the singleton instance for this particular type
        //static const TypeInfo ti(refl::reflect<T>());
        using decayType= typename std::decay<T>::type;
        static const TypeInfo ti(typeid(decayType));
        return ti;
    }

    template <typename T>
    static StringRef getTypeName() {
        auto& ti = TypeInfo::Get<T>();
        return ti.Name();
    }


    std::string demangle(const char* mangle_name) {
        int status = -4;
        std::unique_ptr<char, void(*)(void*)> res {
            abi::__cxa_demangle(mangle_name, NULL, NULL, &status),
            std::free
        };
        return (status == 0) ? res.get() : mangle_name;
    }

    StringRef Name() const
    {
        return name_;
    }

private:

    const std::type_info& type_;

    // were only storing the name for demonstration purposes,
    // but this can be extended to hold other properties as well
    // std::string name_;
    StringRef name_;

    TypeInfo(const std::type_info& ti)
        : type_(ti)
        , name_(demangle(ti.name()))
    {
        //name_ = String::intern(demangle(ti.name()));
    }

};


#endif //#TYPEINFO_H
