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

