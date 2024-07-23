#pragma once
#include <iostream>
#include <tuple>
#include <typeinfo>
#include <utility>
#include <vector>
#include <algorithm>
#include <memory>

// Helper structure to combine types into a single tuple.
template <typename... Ts>
struct TypeList {};

// 获取类型特征码并排序
template <typename... Types>
struct TypeInfoSorter {
    using TupleType = std::tuple<Types...>;

    static std::vector<std::pair<const std::type_info*, size_t>> getSortedTypeInfo() {
        std::vector<std::pair<const std::type_info*, size_t>> typeInfoVec = { {&typeid(Types), sizeof(Types)}... };
        std::sort(typeInfoVec.begin(), typeInfoVec.end(),
            [](const auto& lhs, const auto& rhs) {
                return lhs.first->before(*rhs.first);
            });

        return typeInfoVec;
    }
};

// Helper to print the tuple elements.
template <typename Tuple, std::size_t... Is>
void printTuple(const Tuple& t, std::index_sequence<Is...>) {
    ((std::cout << std::get<Is>(t) << ' '), ...) << std::endl;
}

// Mixin template class
template <typename... Types>
class Mixin {
public:
    using SortedInfo = TypeInfoSorter<Types...>;
    using MixinType = typename SortedInfo::TupleType;

    template <typename... Args>
    Mixin(Args&&... args) 
        : members(std::forward<Args>(args)...) {}

    void print() const {
        printTuple(members, std::index_sequence_for<Types...>{});
    }

    template <typename T>
    T& get() {
        return std::get<T>(members);
    }

    template <typename T>
    const T& get() const {
        return std::get<T>(members);
    }

    template <typename T>
    void set(const T& value) {
        std::get<T>(members) = value;
    }


private:
    MixinType members;
};

//int main() {
    //Mixin<int, double, char> mixin1(1, 3.14, 'a');
    //mixin1.print(); // Output may vary: '1 3.14 a' or some other order

    //// Access members by type
    //mixin1.set<int>(42);
    //mixin1.set<char>('c');
    //mixin1.print(); // Output should reflect updated values
    
    //std::cout << "int: " << mixin1.get<int>() << "\n";
    //std::cout << "double: " << mixin1.get<double>() << "\n";
    //std::cout << "char: " << mixin1.get<char>() << "\n";

    //Mixin<char, double, int> mixin2('b', 2.71, 22);
    //mixin2.print(); // Output may vary: 'b 2.71 22' or some other order

    //// Access members by type
    //mixin2.set<int>(66);
    //mixin2.set<char>('z');
    //mixin2.print(); // Output should reflect updated values

    //std::cout << "int: " << mixin2.get<int>() << "\n";
    //std::cout << "double: " << mixin2.get<double>() << "\n";
    //std::cout << "char: " << mixin2.get<char>() << "\n";

    //return 0;
//}

