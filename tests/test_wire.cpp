#include "doctest/doctest.h"
#include "Wire.h"

TEST_CASE("Wire functionality") {
    Path _wire1 = Path::make("/wire1");
    Path _wire2 = Path::make("/wire2");
    // 添加端口
    Path _port1 = Path::make("/port1");
    Path _port2 = Path::make("/port2");
    Path _port3 = Path::make("/port3");
    Path _port4 = Path::make("/port4");

    auto wire1 = _wire1.make_object<Wire>();
    auto wire2 = _wire2.make_object<Wire>();
    auto port1 = _port1.make_object<Port>();
    auto port2 = _port2.make_object<Port>();
    auto port3 = _port3.make_object<Port>();
    auto port4 = _port4.make_object<Port>();
    //auto port2 = ObjPtr<Port>::make(_port1.getOrCreateObject<Port>(std::cref(generic_obj2))).value();


    wire1->setMasterPort(port1);
    wire1->setSlavePort(port2);

    wire1->bind();

    port1->send(42);

    CHECK(port2->hasData());
    CHECK(port2->receive<int>() == 42);

    wire2->connect_.master_name = "/port3";
    wire2->connect_.slave_name = "/port4";

    wire2->bind();

    port3->send(44);

    CHECK(port4->hasData());
    CHECK(port4->receive<int>() == 44);


    //port2->send(std::string("Hello, World!"));
    //CHECK(port1->hasData());
    //CHECK(port1->receive<std::string>() == "Hello, World!");
}

