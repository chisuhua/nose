#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include "TypeManager.h"
#include "Clock.h"
#include "Component.h"
#include "Channel.h"
#include "IniLoader.h"
#include "Tree.h"
#include "Visitor.h"
#include "ObjectBuildVisitor.h"
#include "PortBindVisitor.h"
#include "ComponentBindVisitor.h"
#include "ChannelBindVisitor.h"
#include "PrinterVisitor.h"
#include "Port.h"
#include "Event.h"
#include "ComponentSrc.h"
#include "ComponentDst.h"

    REGISTER_OBJECT(Wire)
    REGISTER_OBJECT(Channel)
    REGISTER_OBJECT(Clock)

    REGISTER_OBJECT(Port)

int main() {
    TypeManager& typeManager = TypeManager::instance();

    Tree tree;
    IniLoader loader(typeManager);

    try {
        loader.load("samples/core/config.ini", tree);

        ObjectBuildVisitor builderVisitor(typeManager, Registry::getInstance());
        tree.accept(builderVisitor);

        ComponentBindVisitor componentBindVisitor;
        tree.accept(componentBindVisitor);

        PortBindVisitor portBindVisitor(tree);
        tree.accept(portBindVisitor);

        ChannelBindVisitor channelBindVisitor(tree);
        tree.accept(channelBindVisitor);

        auto srcEntity = tree.findEntity("/a/b:ComponentSrc");
        auto srcUnit1 = std::static_pointer_cast<ComponentSrc>(srcEntity->getObject("ComponentSrc"));

        auto dstEntity = tree.findEntity("/a/b/c:ComponentDst");
        auto dstUnit1 = std::static_pointer_cast<ComponentDst>(dstEntity->getObject("ComponentDst"));

        auto clockEntity = tree.findEntity("/a/clock/clock1:Clock");
        auto clock = std::static_pointer_cast<Clock>(clockEntity->getObject("Clock"));

        if (clock) {
            clock->tick();
        }

        std::future<void> future = std::async(std::launch::async, [&, dstUnit1] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // auto dstPort = std::dynamic_pointer_cast<Port<bundle::ValidReady>>(dstUnit1->getPort("port1"));
            auto dstPort = std::dynamic_pointer_cast<Port>(dstUnit1->getPort("port1"));
            //Event io;
            //*dstPort >> io;
            Event io = dstPort->receive<Event>();
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

