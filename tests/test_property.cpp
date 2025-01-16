#include "doctest/doctest.h"
#include <iostream>
#include <string>
#include <variant>
#include <unordered_map>
#include <memory>
#include <functional>
#include <refl.hpp>
#include "IniLoader.h"
#include "Property.h"
#include "Tree.h"
#include "Visitor.h"
#include "Registry.h"
#include "ObjectBuildVisitor.h"
#include "PrinterVisitor.h"

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

class CustomObject {
public:
    int intval;
    float floatval;
    std::string strval;
    Orientation orientation;

};
// Reflexion macro
REFL_AUTO(
    type(CustomObject),
    field(intval, Property()),
    field(floatval, Property()),
    field(strval, Property(&parse_custom)),
    field(orientation, Property(PropertyType::Required, &parse_orientation))
)

class SummationVisitor : public Visitor<void> {
public:
    virtual void visitObject(const std::shared_ptr<void>& obj, const std::string& key) override {
        auto custom_object = std::static_pointer_cast<CustomObject>(obj);
        if (custom_object) {
            // 实现求和逻辑
            totalSum_ += custom_object->intval;
            totalSum_ += static_cast<int>(custom_object->floatval); // 这里粗略求和
        }
    }

    int getTotalSum() const {
        return totalSum_;
    }

private:
    int totalSum_ = 0;
};

REGISTER_OBJECT(CustomObject)

TEST_CASE("Basic Property") {
    // 创建 TypeManager 实例并注册 CustomObject
    TypeManager& typeManager = TypeManager::instance();
    //typeManager.registerType<CustomObject>("CustomObject");

    // 创建 Tree 实例并加载配置
    Tree tree;
    IniLoader loader(typeManager);

    try {
        loader.load("tests/config.ini", tree);

        // 使用 ObjectBuilderVisitor 创建对象
        ObjectBuildVisitor builderVisitor(typeManager, Registry::getInstance());
        tree.accept(builderVisitor);

      // 使用 PrinterVisitor 打印树结构
        PrinterVisitor printerVisitor;
        tree.accept(printerVisitor);

        auto entity = tree.findEntity("a/b");
        CHECK(entity != nullptr);

        auto obj1 = std::static_pointer_cast<CustomObject>(entity->getObject(entity->getName()));
        CHECK(obj1 != nullptr);

        auto obj2 = std::static_pointer_cast<CustomObject>(entity->getChild("c")->getObject("CustomObject"));

        CHECK(obj2 != nullptr);

        CHECK(obj1->intval == 10);
        CHECK(obj2->intval == 30);

        std::cout << obj1->strval << " " << obj2->strval << "\n";

          // 使用 SummationVisitor 求和
        SummationVisitor summationVisitor;
        tree.accept(summationVisitor);
        std::cout << "Total Sum: " << summationVisitor.getTotalSum() << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

