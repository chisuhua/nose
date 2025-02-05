#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
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

void print_orientation(Orientation ori) {
    if (ori == Orientation::Horizontal) {
        std::cout << "Hori\n";
    } else if (ori == Orientation::Vertical) {
        std::cout << "Vert\n";
    } else {
        throw std::runtime_error("invalid Orientation");
    }
}

// Custom parsing function
std::string parse_custom(const std::string& value_str) {
    return "Custom Parsed: " + value_str;
}

struct Person {
  std::string first_name;
  std::string last_name;
  int age;
};

using PersonsType = std::vector<rfl::Ref<Person>>;
PersonsType parse_person(const std::string& value_str) {
    return rfl::json::read<PersonsType>(value_str).value();
}


REFL_AUTO(
    type(Person)
)

class CustomObject {
public:
    int intval;
    float floatval;
    std::string strval;
    Orientation orientation;
    Person person;
    std::vector<rfl::Ref<Person>> persons;

};
// Reflexion macro
REFL_AUTO(
    type(CustomObject),
    field(intval),
    field(floatval),
    field(strval, Property(&parse_custom)),
    field(orientation, Property(PropertyType::Required, &parse_orientation)),
    field(person),
    field(persons, Property(&parse_person))
)

// It is not Generalizable Object
class CustomTop {
public:
    using GenericType = std::shared_ptr<CustomObject>;
    GenericType generic_;

    explicit CustomTop(GenericType generic): generic_(generic) {}

    CustomTop() = delete;

    ~CustomTop() = default;

    const GenericType& GetGeneric() const { return generic_; }
    void setGeneric(GenericType generic) { generic_ = generic; }

};

REFL_AUTO(
    type(CustomTop),
    field(generic_)
)



class SummationVisitor : public Visitor<void> {
public:
    virtual void visitObject(const std::shared_ptr<void>& obj, StringRef, Path) override {
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
REGISTER_OBJECT(Person)
//REGISTER_OBJECT(CustomTop)

TEST_CASE("Basic Property") {
    TypeManager& typeManager = TypeManager::instance();

    //StorageObjectCreator createStorageObjectGeneric = [](Path entity, std::any args) -> std::shared_ptr<void> {
        //auto [first_name, last_name, age] = std::any_cast<std::tuple<std::string, std::string, int>>(args);
        //auto generic_obj = std::make_shared<Person>();
        //generic_obj->first_name = first_name;
        //generic_obj->last_name = last_name;
        //generic_obj->age = age;
        //return std::static_pointer_cast<void>(generic_obj);
    //};

    //typeManager.registerStorageObjectCreator("Person", createStorageObjectGeneric);


    // 创建 Tree 实例并加载配置
    Tree tree;
    IniLoader loader(typeManager);

    try {
        loader.load("tests/config.ini", tree);

        // 使用 ObjectBuilderVisitor 创建对象
        ObjectBuildVisitor builderVisitor(typeManager);
        tree.accept(builderVisitor);

        // 使用 PrinterVisitor 打印树结构
        PrinterVisitor printerVisitor;
        tree.accept(printerVisitor);

        auto path_a_b = tree.findEntity("/a/b");
        CHECK(path_a_b.isValid());

        auto obj1 = path_a_b.getObject<CustomObject>();
        CHECK(obj1 != nullptr);

        auto path_a_b_c = Path("c", path_a_b);

        auto obj2 = path_a_b_c.getObject<CustomObject>();
        CHECK(obj2 != nullptr);

        CHECK(obj1->intval == 30);
        CHECK(obj2->intval == 50);

        std::cout << "intval:" << obj1->intval << " " << obj2->intval << "\n";
        std::cout << "floatval:" << obj1->floatval << " " << obj2->floatval << "\n";
        std::cout << "strval:" << obj1->strval << " " << obj2->strval << "\n";
        print_orientation(obj1->orientation);
        print_orientation(obj2->orientation);
        std::cout << "person:" << obj1->person.first_name << " " << obj2->person.first_name << "\n";

        auto person_a_entity = tree.findEntity("/a/b/person_a");
        auto person_b_entity = tree.findEntity("/a/b/person_b");
        auto person_c_entity = tree.findEntity("/a/b/c/person_c");
        auto person_a = person_a_entity.getObject<Person>();
        auto person_b = person_b_entity.getObject<Person>();
        auto person_c = person_c_entity.getObject<Person>();
        std::cout << "person_a:" << person_a->first_name << " \n";
        std::cout << "person_b:" << person_b->first_name << " \n";
        std::cout << "person_c:" << person_c->first_name << " \n";

        for (auto&& p : obj1->persons) {
            std::cout << "obj1 persons:" << p->first_name << p->last_name << "\n";
        }
        for (auto&& p : obj2->persons) {
            std::cout << "obj2 persons:" << p->first_name << p->last_name << "\n";
        }

        // 使用 SummationVisitor 求和
        SummationVisitor summationVisitor;
        tree.accept(summationVisitor);
        std::cout << "Total Sum: " << summationVisitor.getTotalSum() << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

