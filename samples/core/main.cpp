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
#include "Component.h"
//#include "ComponentDst.h"

struct Event {
    uint32_t data;
    bool valid;
};

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

        // 使用 PrinterVisitor 打印树结构
        PrinterVisitor printerVisitor;
        tree.accept(printerVisitor);


        auto srcEntity = tree.findEntity("/a/b");
        assert(srcEntity.isValid());
        auto srcUnit = srcEntity.getObject<Component>();
        assert(srcUnit.ptr());

        auto dstEntity = tree.findEntity("/a/b/c");
        assert(dstEntity.isValid());
        auto dstUnit = dstEntity.getObject<Component>();
        assert(dstUnit.ptr());

        auto clockEntity = tree.findEntity("/a/clock/clock1");
        assert(clockEntity.isValid());
        auto clock = clockEntity.getObject<Clock>();
        assert(clock.ptr());

        clock->tick();

        std::future<void> future = std::async(std::launch::async, [&, dstUnit] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // auto dstPort = std::dynamic_pointer_cast<Port<bundle::ValidReady>>(dstUnit1->getPort("port1"));
            auto dstPort = dstUnit->getPort("port1");
            //Event io;
            //*dstPort >> io;
            Event io = dstPort->receive<Event>();
            std::cout << "ComponentDst received event: data = " << io.data << ", valid = " << io.valid << std::endl;
        });

        std::cout << "Component sending event: data of Event{42, true}" << std::endl;
        srcUnit->sendPacket(Event{42, true}, "port1");

        future.get();

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::cout << "exsting" << std::endl;
    exit(0);
#endif
    return 0;
}

