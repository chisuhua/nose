#include "Wire.h"
#include <iostream>

int main() {
    // 创建两个 Port 实例
    auto port1 = std::make_unique<Port>(PortRole::Master);
    auto port2 = std::make_unique<Port>(PortRole::Slave);

    // 创建 Wire 实例
    Wire wire;
    wire.connect_.push_back(std::move(port1));
    wire.connect_.push_back(std::move(port2));

    // 绑定端口
    wire.Bind();

    // 通过 port1 发送 int 类型的数据
    wire.connect_[0]->send(42);
    wire.connect_[0]->send(43);

    // 通过 port2 接收 int 类型的数据
    while (wire.connect_[1]->hasData()) {
        int receivedInt = wire.connect_[1]->receive<int>();
        std::cout << "Received int: " << receivedInt << std::endl;
    }

    // 通过 port2 发送 std::string 类型的数据
    wire.connect_[1]->send(std::string("Hello, World!"));
    wire.connect_[1]->send(std::string("Another message"));

    // 通过 port1 接收 std::string 类型的数据
    while (wire.connect_[0]->hasData()) {
        std::string receivedString = wire.connect_[0]->receive<std::string>();
        std::cout << "Received string: " << receivedString << std::endl;
    }

    // 克隆 port1
    auto clonedPort1 = wire.connect_[0]->clone();
    clonedPort1->bind(wire.connect_[1].get());

    // 通过克隆的 port1 发送数据
    clonedPort1->send(100);

    // 通过 port2 接收数据
    while (wire.connect_[1]->hasData()) {
        int receivedInt = wire.connect_[1]->receive<int>();
        std::cout << "Received int from cloned port: " << receivedInt << std::endl;
    }

    return 0;
}

