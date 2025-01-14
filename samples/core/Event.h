#pragma once
#include "Port.h"
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
        PortRole getRole() { return PortRole::Master; }
    };

    template <>
    PortRole ValidReady::getRole<RSP>() { return PortRole::Slave; };

}
