#include "doctest/doctest.h"
#include "Component.h"

TEST_CASE("Component functionality") {
    Path entity_port1 = Path::make("/port1");
    Path entity_port2 = Path::make("/port2");
    Path entity_component = Path::make("/component");

    auto port1 = entity_port1.make_object<Port>();
    auto port2 = entity_port2.make_object<Port>();
    auto component = entity_component.make_object<Component>();

    component->addPort("port1", port1);
    component->addPort("port2", port2);

    port1->bind(port2);
    // 模拟端口变化
    port1->send(42);
    //port2->send(std::string("Hello, World!"));

    // 通知组件端口变化
    component->portNotified("port1");
    component->portNotified("port2");

    // 模拟时钟更新
    component->tick();

    // 检查端口是否已更新
    CHECK(component->isPortUpdated("port1") == true);
    CHECK(component->isPortUpdated("port2") == true);


    port1->send(42);
    CHECK(port2->hasData());
    CHECK(port2->receive<int>() == 42);

    //port2->send(std::string("Hello, World!"));
    //CHECK(port1->hasData());
    //CHECK(port1->receive<std::string>() == "Hello, World!");
}

