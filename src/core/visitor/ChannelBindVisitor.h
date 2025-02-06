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

    void visit(Path entity, int level) override {
        auto channelObject = entity.getObject("Channel");
        if (channelObject.ptr()) {
            auto channel = channelObject.as<Channel>();
            auto master = channel->getMasterPort();
            master->addObserver([channel, portName = master->getRole() == PortRole::Master ? "Master" : "Slave"]() {
                channel->portNotified(portName);
            });

            auto slave = channel->getMasterPort();
            slave->addObserver([channel, portName = slave->getRole() == PortRole::Master ? "Master" : "Slave"]() {
                channel->portNotified(portName);
            });
        }

        Visitor<void>::visit(entity, level);
    }

    void visitObject(const std::shared_ptr<void>&, StringRef, Path) override {}

private:
    Tree& tree_;
};

#endif // CHANNELBINDVISITOR_H
