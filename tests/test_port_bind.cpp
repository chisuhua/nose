#include "doctest/doctest.h"
#include "PortBindVisitor.h"
#include "Channel.h"
#include "Port.h"
#include "Entity.h"
#include "Tree.h"
#include <iostream>

TEST_CASE("PortBindVisitor functionality") {
    // 创建 Tree 实例
    Tree tree;

    // 创建 Entity 实例并添加到 Tree 中
    auto entity1 = std::make_shared<Entity>("entity");
    auto channel1 = std::make_shared<Channel>();
    channel1->connect_.push_back(std::make_unique<Port>(PortRole::Master));
    channel1->connect_.push_back(std::make_unique<Port>(PortRole::Slave));
    entity1->setObject("Channel", channel1);
    tree.setCurrent(entity1);

    // 创建 PortBindVisitor 实例
    PortBindVisitor visitor(tree);

    // 访问 Tree 中的所有 Entity
    tree.accept(visitor);

    // 绑定端口
    channel1->Bind();

    // 模拟端口变化
    channel1->masterPort_->send(42);
    channel1->slavePort_->send(std::string("Hello, World!"));

    // 通知观察者
    channel1->masterPort_->notifyObservers();
    channel1->slavePort_->notifyObservers();
}

