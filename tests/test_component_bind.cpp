#include "doctest/doctest.h"
#include "ComponentBindVisitor.h"
#include <iostream>

TEST_CASE("ComponentBindVisitor functionality") {
    // 创建 Tree 实例
    Tree tree;

    // 创建 Entity 实例并添加到 Tree 中
    auto entity1 = std::make_shared<Entity>("entity");
    auto component1 = std::make_shared<Component>();
    auto port1 = std::make_shared<Port>(PortRole::Master);
    auto port2 = std::make_shared<Port>(PortRole::Slave);
    component1->addPort("port1", port1);
    component1->addPort("port2", port2);
    entity1->setObject("Component", component1);
    tree.setCurrent(entity1);

    // 创建 ComponentBindVisitor 实例
    ComponentBindVisitor visitor;

    // 访问 Tree 中的所有 Entity
    tree.accept(visitor);

    // 模拟端口变化
    port1->send(42);
    port2->send(std::string("Hello, World!"));

    // 通知组件端口变化
    component1->portNotified("port1");
    component1->portNotified("port2");

    // 模拟时钟更新
    component1->tick();

    // 检查端口是否已更新
    CHECK(component1->portsUpdated_["port1"] == true);
    CHECK(component1->portsUpdated_["port2"] == true);
}

