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
    using SortedTypesList = typename SortedInfo::TupleType;

    template <typename... Args>
    Mixin(Args&&... args) : members(std::forward<Args>(args)...) {}

    void print() const {
        printTuple(members, std::index_sequence_for<Types...>{});
    }

private:
    SortedTypesList members;
};

//int main() {
    //Mixin<int, double, char> mixin1(1, 3.14, 'a');
    //mixin1.print(); // Sorted: char, double, int

    //Mixin<char, double, int> mixin2('b', 2.71, 42);
    //mixin2.print(); // Sorted: char, double, int

    //return 0;
//}

