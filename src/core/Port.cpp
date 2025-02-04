#include <refl.hpp>
#include "Port.h"
#include "Property.h"

PortRole parse_role(std::string_view str) {
    if (str == "Master") return PortRole::Master;
    if (str == "Slave") return PortRole::Slave;
    throw std::runtime_error("Cannot parse " + std::string(str) + " as PortRole");
}

void Port::bind(ObjPtr<Port> peer) {
    generic_->peer_ = peer; // ->getGeneric().cast_to<GenericType>();
}

void Port::setRole(PortRole role) {
    generic_->role_ = role;
}

PortRole Port::getRole() const {
    return generic_->role_;
}

//void Port::addData(rfl::Generic data) {
void Port::addData(RflGeneric data) {
    generic_->dataQueue_.push_back(data);
}

bool Port::hasData() const {
    return !generic_->dataQueue_.empty();
}


ObjPtr<Port> Port::peer() {
    //assert(generic_->peer_);
    return generic_->peer_;
    //return std::dynamic_pointer_cast<Port>(this->generic_->peer_);
}

RflGeneric Port::receiveData() {
    if (!generic_->dataQueue_.empty()) {
        RflGeneric data = generic_->dataQueue_.front();
        generic_->dataQueue_.pop_front();
        return data;
    }
    throw std::runtime_error("No data available to receive");
}






