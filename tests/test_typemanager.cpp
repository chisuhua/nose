#include "doctest/doctest.h"
#include <string>
#include <memory>
#include <refl.hpp>
#include "Property.h"
#include "Port.h"
#include "TypeManager.h"

namespace test_typemanager {
// Simple enum and its parsing function
enum class Orientation {
    Horizontal,
    Vertical,
};

Orientation parse_orientation(std::string_view str) {
    if (str == "horizontal") return Orientation::Horizontal;
    if (str == "vertical") return Orientation::Vertical;
    throw std::runtime_error("Cannot parse " + std::string(str) + " as Orientation");
}

// Custom parsing function
std::string parse_custom(const std::string& value_str) {
    return "Custom Parsed: " + value_str;
}

class CustomObjectGeneric {
public:
    int intval;
    float floatval;
    std::string strval;
    Orientation orientation;
};

class CustomObject {
public:
    using GenericType = std::shared_ptr<CustomObjectGeneric>;
    GenericType generic_;
    CustomObject(GenericType generic) : generic_(generic) {
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
}

// Reflexion macro
REFL_AUTO(
    type(test_typemanager::CustomObject),
    field(intval, Property()),
    field(floatval, Property()),
    field(strval, Property(&test_typemanager::parse_custom)),
    field(orientation, Property(PropertyType::Required, &test_typemanager::parse_orientation))
)

//REGISTER_OBJECT(CustomObject)
using namespace test_typemanager;

PortRole parse_role(std::string_view str) {
    if (str == "Master") return PortRole::Master;
    if (str == "Slave") return PortRole::Slave;
    throw std::runtime_error("Cannot parse " + std::string(str) + " as PortRole");
}

TEST_CASE("Test registerStorageObjectCreator and createStorageObject") {
    TypeManager& typeManager = TypeManager::instance();

    // 注册 CustomObjectGeneric 的存储对象创建器
    StorageObjectCreator createStorageObjectGeneric = [](EntityRef entity, std::any args) -> std::shared_ptr<void> {
        auto [intval, floatval, strval] = std::any_cast<std::tuple<int, float, std::string>>(args);
        auto generic_obj = std::make_shared<CustomObjectGeneric>();
        generic_obj->intval = intval;
        generic_obj->floatval = floatval;
        generic_obj->strval = strval;
        generic_obj->orientation = Orientation::Horizontal; // 默认值
        return std::static_pointer_cast<void>(generic_obj);
    };

    typeManager.registerStorageObjectCreator("CustomObjectGeneric", createStorageObjectGeneric);

    // 注册 CustomObject 的存储对象创建器
    StorageObjectCreator createStorageObject = [](EntityRef entity, std::any args) -> std::shared_ptr<void> {
        auto generic = std::any_cast<std::shared_ptr<CustomObjectGeneric>>(args);
        auto custom_obj = std::make_shared<CustomObject>(generic);
        return std::static_pointer_cast<void>(custom_obj);
    };

    typeManager.registerStorageObjectCreator("CustomObject", createStorageObject);

    // 测试创建 CustomObjectGeneric 存储对象
    EntityRef entity_generic = EntityRef::make("/test_entity_generic");
    auto generic_obj = typeManager.createStorageObject("CustomObjectGeneric", entity_generic, std::make_tuple(10, 3.14f, "hello"));
    auto generic_obj_ptr = std::static_pointer_cast<CustomObjectGeneric>(generic_obj);
    CHECK(generic_obj_ptr->intval == 10);
    CHECK(generic_obj_ptr->floatval == 3.14f);
    CHECK(generic_obj_ptr->strval == "hello");
    CHECK(generic_obj_ptr->orientation == Orientation::Horizontal);

    // 测试创建 CustomObject 存储对象
    EntityRef entity = EntityRef::make("/test_entity");
    auto custom_obj = typeManager.createStorageObject("CustomObject", entity, generic_obj_ptr);
    auto custom_obj_ptr = std::static_pointer_cast<CustomObject>(custom_obj);
    CHECK(custom_obj_ptr->intval == 10);
    CHECK(custom_obj_ptr->floatval == 3.14f);
    CHECK(custom_obj_ptr->strval == "hello");
    CHECK(custom_obj_ptr->orientation == Orientation::Horizontal);
}
