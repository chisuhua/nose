#ifndef PORT_H
#define PORT_H

#include <memory>
#include <refl.hpp>
#include "Object.h"
#include "ObjPtr.h"
#include "Path.h"
#include "Property.h"
#include "ObjPtr.h"
#include "Registry.h"

enum class PortRole { Master, Slave };
PortRole parse_role(std::string_view str) ;

struct PortData;

class Port {
public:
    using GenericType = std::shared_ptr<PortData>;
    GenericType generic_;

    explicit Port(GenericType generic) 
        : generic_(generic) {}


    void bind(ObjPtr<Port> peer) ;

    void setRole(PortRole role) ;

    PortRole getRole() const ;

    void addData(RflGeneric data) ;
    bool hasData() const ;

    RflGeneric receiveData() ;

    ObjPtr<Port> peer() ;

    template <typename T>
    void send(const T& data) {
        peer()->addData(rfl::to_generic(data));
    }

    template <typename T>
    T receive() {
        if (hasData()) {
            T data = rfl::from_generic<T>(receiveData()).value();
            return data;
        }
        throw std::runtime_error("No data available to receive");
    }

    void addObserver(std::function<void()> observer) {
        observers_.push_back(observer);
    }

    void notifyObservers() {
        for (const auto& observer : observers_) {
            observer();
        }
    }

    // 观察者列表
    std::vector<std::function<void()>> observers_;

    //Port() = default;
    //Port& operator=(const Port&) = delete;
};

struct PortData {
    using OwnerType = Port;
    EntityHashType entity_hash;
    ObjPtr<Port> peer_;
    //std::shared_ptr<ObjectRef> peer_;
    PortRole role_;
    //std::deque<rfl::Generic> dataQueue_;
    std::deque<RflGeneric> dataQueue_;
};



 REFL_AUTO(
    type(Port),
    field(generic_)
    );
REGISTER_OBJECT(Port);
//static ObjectRegistrar<Port> auto_Port_("Port");

#endif // PORT_H
