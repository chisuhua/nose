#include <refl.hpp>
#include <Registry.h>
#include <type_traits>

template<typename Type>
struct in_place_delete {
    static constexpr bool value = !std::is_move_constructible<Type>::value && !std::is_move_assignable<Type>::value;
};

template<>
struct in_place_delete<void> : std::false_type {};
// 用于全局注册的工具模板类
template <typename O, typename E>
struct ObjectRegistrar {
    ObjectRegistrar(const std::string& type_name) {
        // registerObjectTypeHelper<T, E>(typeName);
        Registry::getInstance()->registerObjectType<O, E>(type_name);
    }
};
class EntityNull {
    using ObjectId = uint32_t;
    using EntityId = uint32_t;
}

#define REGISTER_NODE_OBJECT(O, ...) \
    REFL_AUTO(type(O), __VA_ARGS__) \
    static ObjectRegistrar<O, Node> auto_##O##_registrar(#O);

#define REGISTER_OBJECT(O, ...) \
    REFL_AUTO(type(O), __VA_ARGS__) \
    static ObjectRegistrar<O, EntityNull> auto_##O##_registrar(#O);

