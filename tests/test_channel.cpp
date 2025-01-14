#include "doctest/doctest.h"
#include "Channel.h"
#include <iostream>

TEST_CASE("Channel functionality") {
    // 创建 Channel 实例
    Channel channel;

    // 添加端口
    auto port1 = std::make_unique<Port>(PortRole::Master);
    auto port2 = std::make_unique<Port>(PortRole::Slave);
    channel.connect_.push_back(std::move(port1));
    channel.connect_.push_back(std::move(port2));

    // 绑定端口
    channel.Bind();

    // 发送和接收数据
    channel.masterPort_->send(42);
    CHECK(channel.slavePort_->hasData());
    CHECK(channel.slavePort_->receive<int>() == 42);

    channel.slavePort_->send(std::string("Hello, World!"));
    CHECK(channel.masterPort_->hasData());
    CHECK(channel.masterPort_->receive<std::string>() == "Hello, World!");

    // 模拟时钟更新
    channel.tick();
}

