#include <iostream>
#include <string>
#include <variant>
#include <unordered_map>
#include <memory>
#include <regex>
#include <any>
#include <functional>
#include <stdexcept>
#include <typeindex>
#include <refl.hpp>

// Property 模板结构定义略

// 默认解析器定义略

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

    // Reflexion macro
    REFL_AUTO(
        type(CustomObject),
        field(intval, Property()),
        field(floatval, Property()),
        field(strval, Property(&parse_custom)),
        field(orientation, Property(PropertyType::Required, &parse_orientation))
    )
};

// TypeManager 类定义略

// Node 类定义略

// Tree 类定义略

// PathUtils 类定义略

// IniLoader 类定义略

class PrinterVisitor : public Visitor {
public:
    void visit(Node& node) override {
        if (node.getType() == Node::NodeType::LEAF) {
            visitLeaf(node);
        } else {
            visitDirectory(node);
        }
    }

    void visitDirectory(Node& node) override {
        printNode(node, 0);
    }

    void visitLeaf(Node& node) override {
        printNode(node, 0);
    }

private:
    void printNode(Node& node, int level) {
        std::string indent(level * 2, ' ');
        std::cout << indent << node.getName() << "\n";

        for (const auto& child : node.getChildren()) {
            if (child.second->getType() == Node::NodeType::LEAF) {
                printNode(*child.second, level + 1);
            } else {
                printNode(*child.second, level + 1);
            }
        }
    }
};

class SummationVisitor : public Visitor {
public:
    void visit(Node& node) override {
        if (node.getType() == Node::NodeType::LEAF) {
            visitLeaf(node);
        } else {
            visitDirectory(node);
        }
    }

    void visitDirectory(Node& node) override {
        for (const auto& child : node.getChildren()) {
            child.second->accept(*this);
        }
    }

    void visitLeaf(Node& node) override {
        auto instance = node.getObject();
        if (instance) {
            // 实现求和逻辑
            if (std::shared_ptr<CustomObject> obj = std::dynamic_pointer_cast<CustomObject>(instance)) {
                totalSum_ += obj->intval;
                totalSum_ += static_cast<int>(obj->floatval); // 这里粗略求和
            }
        }
    }

    int getTotalSum() const {
        return totalSum_;
    }

private:
    int totalSum_ = 0;
};

int main() {
    // 创建 TypeManager 实例并注册 CustomObject
    TypeManager& typeManager = TypeManager::instance();
    typeManager.registerType<CustomObject>("CustomObject");

    // 创建 Tree 实例并加载配置
    Tree tree;
    IniLoader loader(typeManager);

    try {
        loader.load("config.ini", tree);

        // 使用 ObjectBuilderVisitor 创建对象
        ObjectBuilderVisitor builderVisitor(typeManager);
        tree.accept(builderVisitor);

        // 使用 PrinterVisitor 打印树结构
        PrinterVisitor printerVisitor;
        tree.accept(printerVisitor);

        // 使用 SummationVisitor 求和
        SummationVisitor summationVisitor;
        tree.accept(summationVisitor);
        std::cout << "Total Sum: " << summationVisitor.getTotalSum() << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

