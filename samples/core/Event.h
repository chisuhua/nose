#pragma once
#include "IPort.h"
#include "Bundle.h"

struct Event {
    uint32_t data;
    bool valid;
};

struct RSP  {
    bool ready;
};

namespace bundle {
    struct ValidReady : public Bundle<ValidReady, Event, RSP> {
        template <typename T>
        Role getRole() { return Role::Master; }
    };

    template <>
    Role ValidReady::getRole<RSP>() { return Role::Slave; };

}
