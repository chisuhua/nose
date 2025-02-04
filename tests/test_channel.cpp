#include "doctest/doctest.h"
#include "Channel.h"
#include <iostream>

TEST_CASE("Channel functionality") {
    Path entity_channel = Path::make("/channel");

    Path entity_port1 = Path::make("/port1");
    Path entity_port2 = Path::make("/port2");

    // 创建 Channel 实例
    auto channel = entity_channel.make_object<Channel>();
    auto port1 = entity_port1.make_object<Port>();
    auto port2 = entity_port2.make_object<Port>();

    channel->setMasterPort(port1);
    channel->setSlavePort(port2);

    // 绑定端口
    channel->bind();

    // 发送和接收数据
    channel->getMasterPort()->send(42);
    CHECK(channel->getSlavePort()->hasData());
    CHECK(channel->getSlavePort()->receive<int>() == 42);

    //channel->getSlavePort()->send(std::string("Hello, World!"));
    //CHECK(channel->getMasterPort()->hasData());
    //CHECK(channel->getMasterPort()->receive<std::string>() == "Hello, World!");

    // 模拟时钟更新
    //channel->tick();
}

