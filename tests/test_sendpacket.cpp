#include "doctest/doctest.h"
#include <iostream>
#include <fstream>
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
#include "ObjectRemoveVisitor.h"
#include "PrinterVisitor.h"
#include "Port.h"
//#include "Event.h"
//#include "ComponentSrc.h"
//#include "ComponentDst.h"

struct Event {
    uint32_t data;
    bool valid;
};



//int main() {
TEST_CASE("SendPacket") {
    TypeManager& typeManager = TypeManager::instance();
    Tree tree;
    IniLoader loader(typeManager);

    std::ofstream ofile("test_SendPacket_config.ini");
    CHECK(ofile);

    ofile << 
R"([/a/b:Component]
port1:Port/role_ = master

[/a/b/c:Component]
port1:Port/role_ = slave
port_link:Port/role_ = master

[/a/b/d:Component]
port_link:Port/role_ = slave

[/a/connect]
wire:Wire/connect_ = {"master_name":"/a/b/port1", "slave_name":"/a/b/c/port1"}
link1:Channel/connect_ = {"master_name":"/a/b/c/port_link", "slave_name":"/a/b/d/port_link"}

[/a/clock/clock1:Clock]
freq_ = 1G
components_ = ["/a/b", "/a/b/c", "/a/b/d"]
channels_ = ["/a/connect/link1"])" << std::endl;

    ofile.close();

    try {
        PrinterVisitor printerVisitor;
        tree.accept(printerVisitor);

        ObjectRemoveVisitor object_remover;
        tree.accept(object_remover);

        tree.accept(printerVisitor);

        loader.load("test_SendPacket_config.ini", tree);

        ObjectBuildVisitor builderVisitor(typeManager);
        tree.accept(builderVisitor);

        ComponentBindVisitor componentBindVisitor;
        tree.accept(componentBindVisitor);

        WireBindVisitor portBindVisitor(tree);
        tree.accept(portBindVisitor);

        ChannelBindVisitor channelBindVisitor(tree);
        tree.accept(channelBindVisitor);

        // 使用 PrinterVisitor 打印树结构
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
            CHECK(io.data == 42);
            CHECK(io.valid == true);
        });

        std::cout << "Component sending event: data of Event{42, true}" << std::endl;
        srcUnit->sendPacket(Event{42, true}, "port1");

        future.get();

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        CHECK(false);
    }

    ObjectRemoveVisitor object_remover;
    tree.accept(object_remover);
}

