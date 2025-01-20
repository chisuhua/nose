#ifndef CHANNELBINDVISITOR_H
#define CHANNELBINDVISITOR_H

#include "Visitor.h"
#include "Tree.h"
#include "Channel.h"
#include <memory>
#include <string>

class ChannelBindVisitor : public Visitor<void> {
public:
    explicit ChannelBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(EntityRef entity, int level) override {
        auto channelObject = entity.getObject("Channel");
        if (channelObject) {
            auto channel = std::static_pointer_cast<Channel>(channelObject);
            for (const auto& portPair : channel->connect_) {
                auto port = portPair.get();
                if (port) {
                    // 注册观察者，当端口发生变化时通知通道
                    port->addObserver([channel, portName = port->getRole() == PortRole::Master ? "Master" : "Slave"]() {
                        channel->portNotified(portName);
                    });
                }
            }
        }

        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef) override {}

private:
    Tree& tree_;
};

#endif // CHANNELBINDVISITOR_H

