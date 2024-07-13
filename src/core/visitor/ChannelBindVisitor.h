// ChannelBindVisitor.hpp
#include "Visitor.h"
#include "Tree.h"
#include "Channel.h"

class ChannelBindVisitor : public Visitor<void> {
public:
    explicit ChannelBindVisitor(Tree& tree) : tree_(tree) {}

    void visit(Node& node) override {
        auto channelObject = node.getObject("Channel");
        if (channelObject) {
            auto channel = std::dynamic_pointer_cast<Channel>(channelObject);
            for (const auto& portPair : channel->connect) {
                auto port = std::dynamic_pointer_cast<IPort>(portPair);
                if (port) {
                    port->addObserver([channel, portName = port->getName()]() {
                        channel->portNotified(portName);
                    });
                }
            }
        }

        for (const auto& [key, child] : node.getChildren()) {
            visit(*child);
        }
    }

    void visitObject(const std::shared_ptr<void>&, const std::string&) override {}

private:
    Tree& tree_;
};

