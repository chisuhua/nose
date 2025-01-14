#include "doctest/doctest.h"
#include "Wire.h"
#include <iostream>

TEST_CASE("Wire functionality") {
    // 创建 Wire 实例
    Wire wire;

    // 添加端口
    auto port1 = std::make_unique<Port>(PortRole::Master);
    auto port2 = std::make_unique<Port>(PortRole::Slave);
    wire.connect_.push_back(std::move(port1));
    wire.connect_.push_back(std::move(port2));

    // 绑定端口
    wire.Bind();

    // 发送和接收数据
    wire.connect_[0]->send(42);
    CHECK(wire.connect_[1]->hasData());
    CHECK(wire.connect_[1]->receive<int>() == 42);

    wire.connect_[1]->send(std::string("Hello, World!"));
    CHECK(wire.connect_[0]->hasData());
    CHECK(wire.connect_[0]->receive<std::string>() == "Hello, World!");
}

