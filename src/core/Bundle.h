#pragma once
#include "refl.hpp"
#include <cassert>

template <typename T>
struct value_proxy : refl::runtime::proxy<value_proxy<T>, T>
{
    // refl::runtime::proxy allows the user to provide their own
    // storage method for the data.
    T target;

    // Provide constructors for value_proxy.
    constexpr value_proxy(const T& target)
        : target(target)
    {
    }

    constexpr value_proxy(T&& target)
        : target(std::move(target))
    {
    }

    template <typename... Args>
    constexpr value_proxy(Args&&... args)
        : target(std::forward<Args>(args)...)
    {
    }

    // static invoke_impl<Member>(Self&&, Args&&...) is the only required
    // method for proxy to work. It gets called whenever a member of value_proxy
    // is called. The result of the call is whatever is returned from invoke_impl.
    template <typename Member, typename Self, typename... Args>
    static constexpr decltype(auto) invoke_impl(Self&& self, Args&&... args)
    {
        // Create an instance of Member to support utility functions.
        constexpr Member member{};
        // Print information about which member is being called.
        std::cout << "Calling " << get_debug_name(member) << " with ";
        // Print all arguemnts provided to that method.
        refl::runtime::debug_all(std::cout, args...);
        std::cout << '\n';

        if constexpr (is_field(member)) {
            static_assert(sizeof...(Args) <= 1, "Invalid number of arguments provided for property!");
            // One argument means that this is a setter-style method.
            if constexpr (sizeof...(Args) == 1) {
                // Check if the value is writable.
                static_assert(is_writable(member));
                // Assign the value. Use identity to instruct that there is only a single argument.
                member(self.target, std::forward<Args>(args)...);
            }
            // Zero arguments means a get method.
            else {
                // Check if the value is readable.
                static_assert(is_readable(member));
                // Return a const reference to the value.
                return refl::util::make_const(member(self.target));
            }
        }
        else {
            // If the member is not a property, simply delegate the call
            // to its original implementation.
            return member(self.target, std::forward<Args>(args)...);
        }
    }
};


#define BUNDLE(TypeName, ...) \
    struct TypeName : public  Mixin<__VA_ARGS__> { \
    }; \
    namespace refl_impl::metadata { template<> struct type_info__<TypeName> { \
        REFL_DETAIL_TYPE_BODY((TypeName), __VA_ARGS__)




