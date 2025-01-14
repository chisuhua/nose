#include "Channel.h"
#include <iostream>

int main() {
    // 创建两个 Port 实例
    auto port1 = std::make_unique<Port>(PortRole::Master);
    auto port2 = std::make_unique<Port>(PortRole::Slave);

    // 创建 Channel 实例
    Channel channel;
    channel.connect_.push_back(std::move(port1));
    channel.connect_.push_back(std::move(port2));

    // 绑定端口
    channel.Bind();

    // 通过 masterPort_ 发送 int 类型的数据
    channel.masterPort_->send(42);
    channel.masterPort_->send(43);

    // 通过 slavePort_ 接收 int 类型的数据
    while (channel.slavePort_->hasData()) {
        int receivedInt = channel.slavePort_->receive<int>();
        std::cout << "Received int: " << receivedInt << std::endl;
    }

    // 通过 slavePort_ 发送 std::string 类型的数据
    channel.slavePort_->send(std::string("Hello, World!"));
    channel.slavePort_->send(std::string("Another message"));

    // 通过 masterPort_ 接收 std::string 类型的数据
    while (channel.masterPort_->hasData()) {
        std::string receivedString = channel.masterPort_->receive<std::string>();
        std::cout << "Received string: " << receivedString << std::endl;
    }

    // 调用 tick 和 portNotified 方法
    channel.tick();
    channel.portNotified("masterPort_");

    return 0;
}

