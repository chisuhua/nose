#include "doctest/doctest.h"
#include "ComponentBindVisitor.h"
#include "Component.h"
#include "Port.h"
#include "Wire.h"
#include "Tree.h"
#include "PrinterVisitor.h"
#include "ObjectRemoveVisitor.h"

TEST_CASE("ComponentBindVisitor functionality") {
    Tree tree;
    Path _root = tree.getRoot(); 
    Path _compA = Path("componentA", _root);
    Path _portA1 = Path("port1", _compA);
    Path _portA2 = Path("port2", _compA);
    Path _port1 = Path("port1", _root);
    Path _port2 = Path("port2", _root);

    Path _wire1 = Path::make("/wire1");
    Path _wire2 = Path::make("/wire2");

    auto componentA = _compA.make_object<Component>();

    auto portA1 = _portA1.make_object<Port>();
    auto portA2 = _portA2.make_object<Port>();
    auto port1 = _port1.make_object<Port>();
    auto port2 = _port2.make_object<Port>();
    auto wire1 = _wire1.make_object<Wire>();
    auto wire2 = _wire2.make_object<Wire>();

    wire1->setMasterPort(port1);
    wire1->setSlavePort(portA1);
    wire1->bind();

    wire2->setMasterPort(port2);
    wire2->setSlavePort(portA2);
    wire2->bind();

    CHECK(componentA->isPortUpdated("port1") == false);
    CHECK(componentA->isPortUpdated("port2") == false);

    // 通知组件端口变化
    componentA->updatePort("port1");
    componentA->updatePort("port2");

    // 检查端口是否已更新
    CHECK(componentA->isPortUpdated("port1") == true);
    CHECK(componentA->isPortUpdated("port2") == true);

    componentA->clearPortUpdate("port1");
    componentA->clearPortUpdate("port2");

    CHECK(componentA->isPortUpdated("port1") == false);
    CHECK(componentA->isPortUpdated("port2") == false);

    PrinterVisitor printerVisitor;
    tree.accept(printerVisitor);

    // 创建 ComponentBindVisitor 实例
    ComponentBindVisitor visitor;

    // 访问 Tree 中的所有 Entity
    tree.accept(visitor);

    // 模拟端口变化
    port1->send(42);
    port2->send(43);

    // 模拟时钟更新
    componentA->tick();

    // 检查端口是否已更新
    CHECK(componentA->isPortUpdated("port1") == true);
    CHECK(componentA->isPortUpdated("port2") == true);

    ObjectRemoveVisitor object_remover;
    tree.accept(object_remover);



}

