#include "doctest/doctest.h"
#include <string>
#include <memory>
#include <refl.hpp>
#include "Property.h"
#include "Port.h"
#include "TypeManager.h"
#include "Registry.h"
#include "ObjectRemoveVisitor.h"
#include "Tree.h"

// Simple enum and its parsing function
enum class Orientation {
    Horizontal,
    Vertical,
};

Orientation parse_orientation2(std::string_view str) {
    if (str == "horizontal") return Orientation::Horizontal;
    if (str == "vertical") return Orientation::Vertical;
    throw std::runtime_error("Cannot parse " + std::string(str) + " as Orientation");
}

// Custom parsing function
std::string parse_custom2(const std::string& value_str) {
    return "Custom Parsed: " + value_str;
}

class Custom2ObjectGeneric {
public:
    int intval;
    float floatval;
    std::string strval;
    Orientation orientation;
};

class Custom2Object {
public:
    using GenericType = std::shared_ptr<Custom2ObjectGeneric>;
    GenericType generic_;
    Custom2Object(GenericType generic) : generic_(generic) {
        intval = generic->intval;
        floatval = generic->floatval;
        strval = generic->strval;
        orientation = generic->orientation;
    }

    int intval;
    float floatval;
    std::string strval;
    Orientation orientation;
};

// Reflexion macro
REFL_AUTO(
    type(Custom2Object),
    field(intval, Property()),
    field(floatval, Property()),
    field(strval, Property(&parse_custom2)),
    field(orientation, Property(PropertyType::Required, &parse_orientation2))
)

REGISTER_OBJECT(Custom2Object)

PortRole parse_role(std::string_view str) {
    if (str == "Master") return PortRole::Master;
    if (str == "Slave") return PortRole::Slave;
    throw std::runtime_error("Cannot parse " + std::string(str) + " as PortRole");
}

TEST_CASE("Test registerStorageObjectCreator and createStorageObject") {
    TypeManager& typeManager = TypeManager::instance();

    using Custom2ObjectGenericArgType = std::tuple<int, float, const char*>;

    // 注册 Custom2ObjectGeneric 的存储对象创建器
    StorageObjectCreator_t<Custom2ObjectGenericArgType> createStorageObjectGeneric = [](Path entity, Custom2ObjectGenericArgType args) -> ObjRef {
        auto [intval, floatval, strval] = args;
        auto generic_obj = entity.make_object<Custom2ObjectGeneric>();
        //auto generic_obj = std::make_shared<Custom2ObjectGeneric>();
        generic_obj->intval = intval;
        generic_obj->floatval = floatval;
        generic_obj->strval = strval;
        generic_obj->orientation = Orientation::Horizontal; // 默认值
        return ObjRef("Custom2ObjectGeneric"_hs, generic_obj.ptr());
    };

    typeManager.registerStorageObjectCreator("Custom2ObjectGeneric", createStorageObjectGeneric);

    using Custom2ObjectGenericPtrType = std::shared_ptr<Custom2ObjectGeneric>;

    // 注册 Custom2Object 的存储对象创建器
    StorageObjectCreator_t<Custom2ObjectGenericPtrType> createStorageObject = [](Path entity, Custom2ObjectGenericPtrType args) -> ObjRef {
        auto generic = std::any_cast<std::shared_ptr<Custom2ObjectGeneric>>(args);
        //auto custom_obj = entity.make_object<Custom2Object>(args);
        auto custom_obj = std::make_shared<Custom2Object>(args);
        return ObjRef("Custom2Object"_hs, custom_obj);
    };

    typeManager.registerStorageObjectCreator("Custom2Object", createStorageObject);

    // 测试创建 Custom2ObjectGeneric 存储对象
    Path entity_generic = Path::make("/test_entity_generic");
    auto generic_obj = typeManager.createStorageObject("Custom2ObjectGeneric", entity_generic, std::make_tuple(10, 3.14f, "hello"));
    auto generic_obj_ptr = generic_obj.as<Custom2ObjectGeneric>();
    CHECK(generic_obj_ptr->intval == 10);
    CHECK(generic_obj_ptr->floatval == 3.14f);
    CHECK(generic_obj_ptr->strval == "hello");
    CHECK(generic_obj_ptr->orientation == Orientation::Horizontal);

    // 测试创建 Custom2Object 存储对象
    Path entity = Path::make("/test_entity");
    auto custom_obj = typeManager.createStorageObject("Custom2Object", entity, generic_obj_ptr);
    auto custom_obj_ptr = custom_obj.as<Custom2Object>();
    CHECK(custom_obj_ptr->intval == 10);
    CHECK(custom_obj_ptr->floatval == 3.14f);
    CHECK(custom_obj_ptr->strval == "hello");
    CHECK(custom_obj_ptr->orientation == Orientation::Horizontal);
    Tree tree;
    ObjectRemoveVisitor object_remover;
    tree.accept(object_remover);



}
