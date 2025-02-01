#include "doctest/doctest.h"
#include "ComponentBindVisitor.h"
#include "Component.h"
#include "Port.h"
#include "Tree.h"

TEST_CASE("ComponentBindVisitor functionality") {
    Tree tree;
    Path entity = tree.getRoot(); 

    // 创建 Entity 实例并添加到 Tree 中
    auto component1 = entity.make_object<Component>();

    auto port1 = entity.make_object<Port>();
    auto port2 = entity.make_object<Port>();

    port1->setRole(PortRole::Master);
    port2->setRole(PortRole::Slave);

    component1->addPort("port1", port1);
    component1->addPort("port2", port2);
    port1->bind(port2);


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
    CHECK(component1->isPortUpdated("port1") == true);
    CHECK(component1->isPortUpdated("port2") == true);
}

