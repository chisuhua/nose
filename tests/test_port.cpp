#include "ObjPtr.h"
#include "doctest/doctest.h"
#include "Path.h"
#include "Port.h"

TEST_CASE("Port functionality") {
    // 创建两个 Port 实例
    Path entity_port1 = Path::make("/port1");
    Path entity_port2 = Path::make("/port2");

    auto port1 = entity_port1.make_object<Port>();
    auto port2 = entity_port2.make_object<Port>();

    port1->setRole(PortRole::Master);
    port2->setRole(PortRole::Slave);


    // 绑定端口
    port1->bind(port2);

    // 发送和接收数据
    port1->send(42);
    CHECK(port2->hasData());
    CHECK(port2->receive<int>() == 42);

    //port2->send(std::string("Hello, World!"));
    //CHECK(port1->hasData());
    //CHECK(port1->receive<std::string>() == "Hello, World!");

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

