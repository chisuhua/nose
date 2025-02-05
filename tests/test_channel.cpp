#include "doctest/doctest.h"
#include "Channel.h"

TEST_CASE("Channel functionality") {
    Path _channel1 = Path::make("/channel1");
    Path _channel2 = Path::make("/channel2");
    // 添加端口
    Path entity_port1 = Path::make("/port1");
    Path entity_port2 = Path::make("/port2");
    Path entity_port3 = Path::make("/port3");
    Path entity_port4 = Path::make("/port4");

    auto channel1 = _channel1.make_object<Channel>();
    auto channel2 = _channel2.make_object<Channel>();
    auto port1 = entity_port1.make_object<Port>();
    auto port2 = entity_port2.make_object<Port>();
    auto port3 = entity_port3.make_object<Port>();
    auto port4 = entity_port4.make_object<Port>();
    //auto port2 = ObjPtr<Port>::make(entity_port1.getOrCreateObject<Port>(std::cref(generic_obj2))).value();


    channel1->setMasterPort(port1);
    channel1->setSlavePort(port2);

    channel1->bind();

    port1->send(42);

    CHECK(port2->hasData());
    CHECK(port2->receive<int>() == 42);

    channel2->connect_.master_name = "/port3";
    channel2->connect_.slave_name = "/port4";

    channel2->bind();

    port3->send(44);

    CHECK(port4->hasData());
    CHECK(port4->receive<int>() == 44);

    // 模拟时钟更新
    channel1->tick();
}

