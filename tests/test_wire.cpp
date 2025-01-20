#include "doctest/doctest.h"
#include "Wire.h"

TEST_CASE("Wire functionality") {
    EntityRef entity_wire("wire");

    Wire::GenericType wire_generic;

    auto generic_wire = rfl::to_generic(wire_generic);

    auto wire = entity_wire.getOrCreateObject<Wire>(std::cref(generic_wire));

    // 添加端口
    EntityRef entity_port1("port1");
    EntityRef entity_port2("port2");

    Port::GenericType port1_generic;
    Port::GenericType port2_generic;

    auto generic_obj1 = rfl::to_generic(port1_generic); // 假设这是一个左值引用或具有足够长生命周期的对象
    auto generic_obj2 = rfl::to_generic(port2_generic); // 假设这是一个左值引用或具有足够长生命周期的对象
                                                       //
    auto port1 = entity_port1.getOrCreateObject<Port>(std::cref(generic_obj1));
    auto port2 = entity_port1.getOrCreateObject<Port>(std::cref(generic_obj2));


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

