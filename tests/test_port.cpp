#include "Port.h"
#include <iostream>

int main() {
    // 创建两个 Port 实例
    auto port1 = std::make_unique<Port>(PortRole::Master);
    auto port2 = std::make_unique<Port>(PortRole::Slave);

    // 将 port1 绑定到 port2
    port1->bind(port2.get());
    port2->bind(port1.get());

    // 通过 port1 发送 int 类型的数据
    port1->send(42);
    port1->send(43);

    // 通过 port2 接收 int 类型的数据
    while (port2->hasData()) {
        int receivedInt = port2->receive<int>();
        std::cout << "Received int: " << receivedInt << std::endl;
    }

    // 通过 port2 发送 std::string 类型的数据
    port2->send(std::string("Hello, World!"));
    port2->send(std::string("Another message"));

    // 通过 port1 接收 std::string 类型的数据
    while (port1->hasData()) {
        std::string receivedString = port1->receive<std::string>();
        std::cout << "Received string: " << receivedString << std::endl;
    }

    // 克隆 port1
    auto clonedPort1 = port1->clone();
    clonedPort1->bind(port2.get());

    // 通过克隆的 port1 发送数据
    clonedPort1->send(100);

    // 通过 port2 接收数据
    while (port2->hasData()) {
        int receivedInt = port2->receive<int>();
        std::cout << "Received int from cloned port: " << receivedInt << std::endl;
    }

    return 0;
}

