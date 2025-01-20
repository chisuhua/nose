#ifndef PORT_H
#define PORT_H

#include <memory>
#include <refl.hpp>
#include "EntityIntern.h"
#include "Property.h"

enum class PortRole { Master, Slave };
PortRole parse_role(std::string_view str) ;

class Port;

struct PortParam {
    EntityHashType entity_hash;
    std::shared_ptr<PortParam> peer_;
    PortRole role_;
    //std::deque<std::any> dataQueue_;
    std::deque<rfl::Generic> dataQueue_;
};

class Port {
public:
    using GenericType = std::shared_ptr<PortParam>;
    GenericType generic_;

    static std::shared_ptr<Port> GetInstance(GenericType generic) {
        auto entity = EntityRef::getEntityByHash(generic->entity_hash);
        return entity.getObject<Port>();
    }

    explicit Port(GenericType generic);
    Port() = delete;
    // TODO fix to delete generic_
    ~Port() = default;

    const GenericType& getGeneric() const { return generic_; }
    void setGeneric(GenericType generic) { generic_ = generic; }

    void bind(std::shared_ptr<Port> peer) {
        generic_->peer_ = peer->getGeneric();
    }

    void setRole(PortRole role) {
        generic_->role_ = role;
    }

    PortRole getRole() const {
        return generic_->role_;
    }

    template <typename T>
    void send(const T& data) {
        assert(generic_->peer_);
        generic_->peer_->dataQueue_.push_back(rfl::to_generic(data));
    }

    template <typename T>
    T receive() {
        if (hasData()) {
            T data = rfl::from_generic<T>(receiveData()).value();
            return data;
        }
        throw std::runtime_error("No data available to receive");
    }

    bool hasData() const {
        return !generic_->dataQueue_.empty();
    }

    //std::unique_ptr<Port> clone() const {
        //return std::make_unique<Port>(*this);
    //}

    void addObserver(std::function<void()> observer) {
        observers_.push_back(observer);
    }

    void notifyObservers() {
        for (const auto& observer : observers_) {
            observer();
        }
    }


    GenericRef receiveData() {
        if (!generic_->dataQueue_.empty()) {
            GenericRef data = generic_->dataQueue_.front();
            generic_->dataQueue_.pop_front();
            return data;
        }
        throw std::runtime_error("No data available to receive");
    }

    //std::shared_ptr<Port> peer_ {nullptr};
    //PortRole role_;
    //std::deque<std::any> dataQueue_;
    //std::deque<GenericRef> dataQueue_;

    // 观察者列表
    std::vector<std::function<void()>> observers_;

    //Port(const Port&) = delete;
    //Port& operator=(const Port&) = delete;
};




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

 REFL_AUTO(
    type(Port),
    field(generic_)
    )



#endif // PORT_H
