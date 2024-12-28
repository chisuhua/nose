#pragma once
#include <tlm.h>

class PayloadPtr {
    tlm::tlm_generic_payload* ptr{nullptr};

public:
    /// @brief Default constructor, creates a unique_ptr that owns nothing.
    PayloadPtr() noexcept = default;

    /// @brief Takes ownership of the pointer.
    inline PayloadPtr(tlm::tlm_generic_payload* p) noexcept
    : ptr(p) {
        if(ptr && ptr->has_mm())
            ptr->acquire();
    }
    /// @brief Copy constructor.
    inline PayloadPtr(PayloadPtr const& p) noexcept
    : ptr(p.ptr) {
        if(ptr && ptr->has_mm())
            ptr->acquire();
    }
    /// @brief Move constructor.´
    inline PayloadPtr(PayloadPtr&& p) noexcept
    : ptr(std::move(p.ptr)) {
        p.ptr = nullptr;
    }
    /// @brief destructor
    ~PayloadPtr() {
        if(ptr && ptr->has_mm())
            ptr->release();
    }
    /// @brief Copy assignment operator.
    PayloadPtr& operator=(PayloadPtr const& p) noexcept {
        if(ptr && ptr->has_mm())
            ptr->release();
        ptr = p.ptr;
        if(ptr && ptr->has_mm())
            ptr->acquire();
        return *this;
    }
    /// @brief Move assignment operator.
    PayloadPtr& operator=(PayloadPtr&& p) noexcept {
        if(ptr && ptr->has_mm())
            ptr->release();
        ptr = p.ptr;
        p.ptr = nullptr;
        return *this;
    }

    /// @brief raw pointer assignment operator.
    PayloadPtr& operator=(tlm::tlm_generic_payload* p) noexcept {
        if(ptr && ptr->has_mm())
            ptr->release();
        ptr = p;
        if(ptr && ptr->has_mm())
            ptr->acquire();
        return *this;
    }

    /// Dereference the stored pointer.
    inline tlm::tlm_generic_payload& operator*() const noexcept { return *ptr; }

    /// Return the stored pointer.
    inline tlm::tlm_generic_payload* operator->() const noexcept { return ptr; }

    /// Return the stored pointer.
    inline tlm::tlm_generic_payload* get() const noexcept { return ptr; }

    inline operator bool() const noexcept { return ptr != nullptr; }
};
inline std::ostream& operator<<(std::ostream& os, PayloadPtr const& p) {
    os << p.get();
    return os;
}
inline bool operator==(PayloadPtr const& x, PayloadPtr const& y) noexcept { return x.get() == y.get(); }

inline bool operator==(PayloadPtr const& x, tlm::tlm_generic_payload* y) noexcept { return x.get() == y; }

inline bool operator!=(PayloadPtr const& x, PayloadPtr const& y) noexcept { return x.get() != y.get(); }

