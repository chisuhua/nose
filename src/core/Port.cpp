#include <refl.hpp>
#include "Port.h"
#include "Property.h"

PortRole parse_role(std::string_view str) {
    if (str == "Master") return PortRole::Master;
    if (str == "Slave") return PortRole::Slave;
    throw std::runtime_error("Cannot parse " + std::string(str) + " as PortRole");
}

//Port::Port(EntityRef entity, GenericRef generic): generic_(Type>(entity, rfl::from_generic<GenericType>(generic).value())) {};

Port::Port(GenericType generic): generic_(generic) {};


