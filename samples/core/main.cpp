#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include "TypeManager.h"
#include "Clock.h"
#include "IniLoader.h"
#include "Tree.h"
#include "ObjectBuildVisitor.h"
#include "WireBindVisitor.h"
#include "ComponentBindVisitor.h"
#include "ChannelBindVisitor.h"
#include "PrinterVisitor.h"
#include "Port.h"
//#include "Event.h"
#include "ComponentSrc.h"
#include "ComponentDst.h"



int main() {
    TypeManager& typeManager = TypeManager::instance();
#if 1
    Tree tree;
    IniLoader loader(typeManager);

    try {
        loader.load("samples/core/config.ini", tree);

        ObjectBuildVisitor builderVisitor(typeManager);
        tree.accept(builderVisitor);

        ComponentBindVisitor componentBindVisitor;
        tree.accept(componentBindVisitor);

        WireBindVisitor portBindVisitor(tree);
        tree.accept(portBindVisitor);

        ChannelBindVisitor channelBindVisitor(tree);
        tree.accept(channelBindVisitor);

        auto srcEntity = tree.findEntity("/a/b:ComponentSrc");
        auto srcUnit1 = srcEntity->getObject("ComponentSrc")->as<ComponentSrc>();

        auto dstEntity = tree.findEntity("/a/b/c:ComponentDst");
        auto dstUnit1 = dstEntity->getObject("ComponentDst")->as<ComponentDst>();

        auto clockEntity = tree.findEntity("/a/clock/clock1:Clock");
        auto clock = clockEntity->getObject("Clock")->as<Clock>();

        if (clock) {
            clock->tick();
        }

        std::future<void> future = std::async(std::launch::async, [&, dstUnit1] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // auto dstPort = std::dynamic_pointer_cast<Port<bundle::ValidReady>>(dstUnit1->getPort("port1"));
            auto dstPort = dstUnit1->getPort("port1");
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
#endif
    return 0;
}

