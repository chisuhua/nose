#include "doctest/doctest.h"
#include "EntityIntern.h"
#include "Port.h"

TEST_CASE("Port functionality") {
    // 创建两个 Port 实例
    EntityRef entity_port1("port1");
    EntityRef entity_port2("port2");

    Port::GenericType port1_generic;
    Port::GenericType port2_generic;

    auto generic_obj1 = rfl::to_generic(port1_generic); // 假设这是一个左值引用或具有足够长生命周期的对象
    auto generic_obj2 = rfl::to_generic(port2_generic); // 假设这是一个左值引用或具有足够长生命周期的对象
                                                       //
    auto port1 = entity_port1.getOrCreateObject<Port>(std::cref(generic_obj1));
    auto port2 = entity_port1.getOrCreateObject<Port>(std::cref(generic_obj2));


    port1->setRole(PortRole::Master);
    port2->setRole(PortRole::Slave);


    // 绑定端口
    port1->bind(port2);

    // 发送和接收数据
    port1->send(42);
    CHECK(port2->hasData());
    CHECK(port2->receive<int>() == 42);

    port2->send(std::string("Hello, World!"));
    CHECK(port1->hasData());
    CHECK(port1->receive<std::string>() == "Hello, World!");

    // 添加观察者
    bool masterNotified = false;
    bool slaveNotified = false;
    port1->addObserver([&masterNotified]() { masterNotified = true; });
    port2->addObserver([&slaveNotified]() { slaveNotified = true; });

    // 通知观察者
    port1->notifyObservers();
    port2->notifyObservers();

    CHECK(masterNotified);
    CHECK(slaveNotified);
}

