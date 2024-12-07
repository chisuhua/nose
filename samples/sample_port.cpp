#include "IPort.h"

// 简单的结构体进行演示
struct A {
    int a1;
    double a2;
};

struct B {
    std::string b1;
    float b2;
};

// 声明反射信息
REFL_AUTO(type(A), field(a1), field(a2))
REFL_AUTO(type(B), field(b1), field(b2))

int main() {
    using io_type = IoLib::IOType<A, B>;

    PortLib::Port<io_type> port_instance_1;
    PortLib::Port<io_type> port_instance_2;

    using extracted_type = IoLib::io_trait<io_type>::type;
    static_assert(std::is_same_v<extracted_type, IoLib::MergeTypes<A, B>>, "Extraction failed");

    extracted_type combined_data;
    combined_data.a1 = 42;
    combined_data.b1 = "Hello";

    port_instance_1 << combined_data;

    extracted_type received_data;
    port_instance_2 >> received_data;

    std::cout << "a1: " << received_data.a1 << ", b1: " << received_data.b1 << std::endl; // 输出a1: 42, b1: Hello

    return 0;
}

