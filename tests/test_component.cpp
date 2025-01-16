#include "doctest/doctest.h"
#include "Component.h"
#include <iostream>

TEST_CASE("Component functionality") {
    // 创建 Component 实例
    Component component;

    // 创建 Port 实例并添加到 Component 中
    auto port1 = std::make_shared<Port>(PortRole::Master);
    auto port2 = std::make_shared<Port>(PortRole::Slave);

    component.addPort("port1", port1);
    component.addPort("port2", port2);

    port1->bind(port2.get());
    // 模拟端口变化
    port1->send(42);
    port2->send(std::string("Hello, World!"));

    // 通知组件端口变化
    component.portNotified("port1");
    component.portNotified("port2");

    // 模拟时钟更新
    component.tick();

    // 检查端口是否已更新
    CHECK(component.portsUpdated_["port1"] == true);
    CHECK(component.portsUpdated_["port2"] == true);


    port1->send(42);
    CHECK(port2->hasData());
    CHECK(port2->receive<int>() == 42);

    port2->send(std::string("Hello, World!"));
    CHECK(port1->hasData());
    CHECK(port1->receive<std::string>() == "Hello, World!");
}

