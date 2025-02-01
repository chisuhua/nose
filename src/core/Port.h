#ifndef PORT_H
#define PORT_H

#include <memory>
#include <refl.hpp>
#include "Object.h"
#include "Path.h"
#include "Property.h"
#include "Object.h"
#include "Registry.h"

enum class PortRole { Master, Slave };
PortRole parse_role(std::string_view str) ;

struct PortGeneric;

class Port {
public:
    using GenericType = std::shared_ptr<PortGeneric>;
    GenericType generic_;

    explicit Port(GenericType generic) 
        : generic_(generic) {}


    void bind(ObjPtr<Port> peer) ;

    void setRole(PortRole role) ;

    PortRole getRole() const ;

    void addData(rfl::Generic data) ;
    bool hasData() const ;

    GenericRef receiveData() ;

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

    Port() = default;
    Port& operator=(const Port&) = delete;
};

struct PortGeneric {
    using OwnerType = Port;
    EntityHashType entity_hash;
    ObjPtr<Port> peer_;
    //std::shared_ptr<ObjectRef> peer_;
    PortRole role_;
    //std::deque<std::any> dataQueue_;
    std::deque<rfl::Generic> dataQueue_;
};



 REFL_AUTO(
    type(Port),
    field(generic_)
    )
REGISTER_OBJECT(Port)

#endif // PORT_H



    // 添加 << 操作符用于send data to port
    //Port<Bundle>& operator<<(const Bundle& data) {
        //assert(peer_);
        //using namespace refl;
        //const auto type_info = reflect<Bundle>();
        //for_each(type_info.members, [this, &data](auto member) {
            //if (peer) {
                //auto& peer_data = peer->data_;
                //const auto& field_prop = refl::descriptor::get_attribute<IoProperty>(member);

                //// 如果自己是 Master，写入对方是 Slave 的成员
                //if (role == Role::Master && field_prop.role == Role::Slave) {
                    //member(peer_data) = member(data);
                    //// always 写入自己的成员
                    //member(this->data_) = member(data);
                //}

                //// 如果自己是 Slave，写入对方是 Master 的成员
                //if (role == Role::Slave && field_prop.role == Role::Master) {
                    //member(peer_data) = member(data);
                    //// alway 写入自己的成员
                    //member(this->data_) = member(data);
                //}
            //}
            //// 无论何种角色，总是写入自己的成员
            ////member(this->io_.io_) = member(data);
        //});
        //return *this;
    //}

    // 添加 >> 操作符用于从port读取数据
    //Port<Bundle>& operator>>(Bundle& data)
        //assert(peer);
        //using namespace refl;
        //const auto type_info = reflect<typename Bundle>();
        //for_each(type_info.members, [this, &data](auto member) {
            //auto& peer_data = peer->data_;
            //const auto& field_prop = refl::descriptor::get_attribute<IoProperty>(member);

            //// 如果自己是 Master，从自身的 Slave 成员读取数据
            //if (role == Role::Master && field_prop.role == Role::Slave) {
                //member(data) = member(this->data_);
            //}

            //// 如果自己是 Slave，从对方的 Master 成员读取数据
            //if (role == Role::Slave && field_prop.role == Role::Master) {
                //member(data) = member(peer_data);
            //}
        //});
        //return *this;
    //}



