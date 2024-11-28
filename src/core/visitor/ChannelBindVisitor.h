#pragma once
#include "Visitor.h"
#include "Tree.h"
#include "Channel.h"

class ChannelBindVisitor : public Visitor<void> {
public:
    explicit ChannelBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(Entity& entity) override {
        auto channelObject = entity.getObject("Channel");
        if (channelObject) {
            auto channel = std::static_pointer_cast<Channel>(channelObject);
            for (const auto& portPair : channel->connect_) {
                auto port = std::static_pointer_cast<IPort>(portPair);
                //if (port) {
                    //port->addObserver([channel, portName = port->getName()]() {
                        //channel->portNotified(portName);
                    //});
                //}
            }
        }

        Visitor<void>::visit(entity);
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}

private:
    Tree& tree_;
};

