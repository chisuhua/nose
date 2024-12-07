#pragma once
#include <tuple>
#include <string>
#include <memory>
#include <numeric> 
#include <typeindex>
#include <array>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <cassert>
#include <refl.hpp>
#include "IPort.h"


template<typename Bundle>
class Port : public IPort, public std::enable_shared_from_this<Port<Bundle>>
{
    using Self = Port<Bundle>;
    using SelfPtr = std::shared_ptr<Self>;
public:
    Port() :peer_(nullptr) {};

    //const Bundle& getIO() const {
        //return io_;
    //}

    //std::any getIO() override {
        //return io_;
    //}

    void bind(std::shared_ptr<IPort> other) override {
        auto derived_other = std::dynamic_pointer_cast<Port<Bundle>>(other);
        if (derived_other) {
            peer_ = derived_other;
        } else {
            throw std::invalid_argument("Incompatible port types for binding");
        }
    }

    std::shared_ptr<IPort> clone() const override {
        auto clonedPort = std::make_shared<Port<Bundle>>();
        clonedPort->data_ = this->data_;
        clonedPort->role = this->role;
        return clonedPort;
    }

    void set_data(const Bundle data) { data_ = data; }
    const Bundle& get_data() const { return data_; }

    template<typename T>
    Port<Bundle>& operator<<(const T& field_value) {
        auto& peer_data = peer_->data_;
        if ((getPortRole() == Role::Master)) {
            assert(peer_data.template getRole<T>() == Role::Slave);
        } else {
            assert(peer_data.template getRole<T>() == Role::Master);
        }
        peer_data.template set(field_value);
        this->data_.template set(field_value);
        return *this;
    }

    template<typename T>
    Port<Bundle>& operator >> (T& field_value) {
        auto& peer_data = peer_->data_;
        if ((getPortRole() == Role::Master)) {
            assert(peer_data.template getRole<T>() == Role::Slave);
        } else {
            assert(peer_data.template getRole<T>() == Role::Master);
        }
        field_value = this->data_.template get<T>();
        return *this;
    }


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

    SelfPtr getPeer() { return peer_; }
private:
    Bundle data_;
    SelfPtr peer_;
};

REFL_AUTO(
        template((typename T), (Port<T>), bases<IPort>)
        );
