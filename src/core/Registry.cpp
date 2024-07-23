#include "refl.hpp"
#include "Registry.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;

template<typename T>
class ObjectCast {
public:
    static std::shared_ptr<T> asTypePtr(std::shared_ptr<void> ptr) {
        return std::static_pointer_cast<T>(ptr);
    }
};


void GeneratePybindBindings(py::module& m) {
    auto registry = Registry::getInstance();
    for(const auto& [type_name, constructor] : registry.objectConstructor) {
        auto type_desc = registry.getTypeDescriptor(type_name);

        // 首先创建一个泛型的Python类
        auto generic_class = py::class_<std::shared_ptr<void>>(m, type_name.c_str());
        // 动态添加构造函数和成员函数
        generic_class.def(py::init([type_name]() {
            auto object = registry.createObjectByName(type_name);
            return object;
        }));

        refl::for_each_field([type_desc, &generic_class](auto& field) {
            // 为每个字段添加绑定，请根据refl-cpp的具体API来调整
            std::string field_name = field.name; // 假设field结构中有name成员

            // 添加字段访问绑定
            generic_class.def_property(field_name.c_str(),
                // getter
                [field_name](std::shared_ptr<void>& ptr) -> auto& {
                    auto castedPtr = ObjectCast<field.declaring_type>::asTypePtr(ptr);
                    return castedPtr->*field.pointer;
                },
                // setter：这个例子假设字段是public的
                // setter：这个例子假设字段是public的
                [field_name](std::shared_ptr<void>& ptr, const decltype(field.type)& value) {
                    auto castedPtr = ObjectCast<field.declaring_type>::asTypePtr(ptr);
                    castedPtr->*field.pointer = value;
                });
        });
    }
}

// Registry g_registry;


PYBIND11_MODULE(core, m) {
    GeneratePybindBindings(m);
}

