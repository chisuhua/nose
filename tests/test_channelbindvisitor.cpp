#include "doctest/doctest.h"
#include "ChannelBindVisitor.h"
#include <iostream>

int main() {
    // 创建 Tree 实例
    Tree tree;

    // 创建 Entity 实例并添加到 Tree 中
    Entity entity1;
    auto channel1 = std::make_shared<Channel>();
    channel1->connect_.push_back(std::make_unique<Port>(PortRole::Master));
    channel1->connect_.push_back(std::make_unique<Port>(PortRole::Slave));
    entity1.addObject("Channel", channel1);
    tree.addEntity(entity1);

    // 创建 ChannelBindVisitor 实例
    ChannelBindVisitor visitor(tree);

    // 访问 Tree 中的所有 Entity
    tree.accept(visitor);

    // 模拟端口变化
    channel1->connect_[0]->send(42);
    channel1->connect_[1]->send(std::string("Hello, World!"));

    // 通知观察者
    channel1->connect_[0]->notifyObservers();
    channel1->connect_[1]->notifyObservers();

    return 0;
}

