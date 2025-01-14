#include "doctest/doctest.h"
#include "Port.h"
#include <iostream>

TEST_CASE("Port functionality") {
    // 创建两个 Port 实例
    auto port1 = std::make_shared<Port>(PortRole::Master);
    auto port2 = std::make_shared<Port>(PortRole::Slave);

    // 绑定端口
    port1->bind(port2.get());

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

