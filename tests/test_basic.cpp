#include "doctest/doctest.h"
#include "TypeManager.h"
#include "Port.h"
//#include "Wire.h"
//#include "Channel.h"
//#include "Clock.h"
//#include "Component.h"
//#include "Event.h"


TEST_CASE("Basic TypeManager registration") {
    TypeManager& typeManager = TypeManager::instance();
    typeManager.registerType<Port>();
    //typeManager.registerType<Wire>();
    //typeManager.registerType<Channel>();
    //typeManager.registerType<Clock>();

    //CHECK(typeManager.getPropertyMeta().count("ValidReady>") == 1);
    CHECK(typeManager.getPropertyMeta().count("Port"_hs) == 1);
    //CHECK(typeManager.getPropertyMeta().count("Wire"_hs) == 1);
    //CHECK(typeManager.getPropertyMeta().count("Channel"_hs) == 1);
    //CHECK(typeManager.getPropertyMeta().count("Clock"_hs) == 1);
    //CHECK(typeManager.getTypeConstructors().count("SrcUnit1") == 1);
    //CHECK(typeManager.getTypeConstructors().count("DstUnit1") == 1);
}

//TEST_CASE("Port creation and binding") {
    //auto port1 = std::make_shared<Port<bundle::ValidReady>>();
    //auto port2 = std::make_shared<Port<bundle::ValidReady>>();

    //port1->setRole(Role::Master);
    //port2->setRole(Role::Slave);

    //port1->bind(port2);

    //CHECK(port1->getPeer() == port2);
    //CHECK(port2->getPeer() == port1);
    //CHECK(port1->getPortRole() == Role::Master);
    //CHECK(port2->getPortRole() == Role::Slave);
//}

//TEST_CASE("Channel creation and notification") {
    //Channel channel;
    //auto port1 = std::make_shared<Port<bundle::ValidReady>>();
    //auto port2 = std::make_shared<Port<bundle::ValidReady>>();

    //port1->setRole(Role::Master);
    //port2->setRole(Role::Slave);
    //channel.connect_ = {port1, port2};

    //channel.Bind();
    //CHECK(channel.masterPort_->getPortRole() == Role::Master);
    //CHECK(channel.slavePort_->getPortRole() == Role::Slave);
//}

