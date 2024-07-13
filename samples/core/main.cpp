#include <iostream>
#include <future>
#include "TypeManager.h"
#include "Clock.h"
#include "Component.h"
#include "Channel.h"
#include "IniLoader.h"
#include "Tree.h"
#include "PortBindVisitor.h"
#include "ComponentBindVisitor.h"
#include "ChannelBindVisitor.h"
#include "Port.h"
//#include "Event.h"
#include "ComponentSrc.h"
#include "ComponentDst.h"

int main() {
    TypeManager& typeManager = TypeManager::instance();
    typeManager.registerTemplateType<Port, IOType<Event, RSP>>("Port<IOType<Event, RSP>>");
    typeManager.registerType<Wire>("Wire");
    typeManager.registerType<Channel>("Channel");
    typeManager.registerType<Clock>("Clock");
    typeManager.registerType<ComponentSrc>("ComponentSrc");
    typeManager.registerType<ComponentDst>("ComponentDst");

    Tree tree;
    IniLoader loader(typeManager);

    try {
        loader.load("config.ini", tree);

        ObjectBuildVisitor builderVisitor(typeManager);
        tree.accept(builderVisitor);

        ComponentBindVisitor componentBindVisitor;
        tree.accept(componentBindVisitor);

        PortBindVisitor portBindVisitor(tree);
        tree.accept(portBindVisitor);

        ChannelBindVisitor channelBindVisitor(tree);
        tree.accept(channelBindVisitor);

        auto srcNode = tree.findNode("/a/b:ComponentSrc");
        auto srcUnit1 = std::static_pointer_cast<ComponentSrc>(srcNode->getObject("ComponentSrc"));

        auto dstNode = tree.findNode("/a/b/c:ComponentDst");
        auto dstUnit1 = std::static_pointer_cast<ComponentDst>(dstNode->getObject("ComponentDst"));

        auto clockNode = tree.findNode("/a/clock/clock1:Clock");
        auto clock = std::static_pointer_cast<Clock>(clockNode->getObject("Clock"));

        if (clock) {
            clock->tick();
        }

        std::future<void> future = std::async(std::launch::async, [&, dstUnit1] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto dstPort = std::dynamic_pointer_cast<Port<IOType<Event, RSP>>>(dstUnit1->getPort("port1"));
            Event io;
            *dstPort >> io;
            std::cout << "ComponentDst received event: data = " << io.data << ", valid = " << io.valid << std::endl;
        });

        if (srcUnit1 && dstUnit1) {
            srcUnit1->sendEvent(Event{42, true}, "port1");
        }

        future.get();

        PrinterVisitor printerVisitor;
        tree.accept(printerVisitor);

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

