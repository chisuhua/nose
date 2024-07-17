#pragma once
#include "IPort.h"

struct Event {
    uint32_t data;
    bool valid;
};

struct RSP  {
    bool ready;
};

REFL_AUTO(
        type(Event),
        field(data, IoProperty<>(Role::Master)),
        field(valid, IoProperty<>(Role::Master))
        )

REFL_AUTO(
        type(RSP),
        field(ready, IoProperty<>(Role::Master))
        )


