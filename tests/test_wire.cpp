#include "doctest/doctest.h"
#include "Wire.h"

TEST_CASE("Wire functionality") {
    Path entity_wire = Path::make("/wire");
    // 添加端口
    Path entity_port1 = Path::make("/port1");
    Path entity_port2 = Path::make("/port2");

    auto wire = entity_wire.make_object<Wire>();
    auto port1 = entity_port1.make_object<Port>();
    auto port2 = entity_port2.make_object<Port>();
    //auto port2 = ObjPtr<Port>::make(entity_port1.getOrCreateObject<Port>(std::cref(generic_obj2))).value();


    wire->setMasterPort(port1);
    wire->setSlavePort(port2);

    wire->bind();

    port1->send(42);

    CHECK(port2->hasData());
    CHECK(port2->receive<int>() == 42);

    port2->send(std::string("Hello, World!"));
    CHECK(port1->hasData());
    CHECK(port1->receive<std::string>() == "Hello, World!");
}

