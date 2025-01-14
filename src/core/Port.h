//#include <tuple>
//#include <string>
//#include <memory>
//#include <numeric> 
//#include <typeindex>
//#include <array>
//#include <algorithm>
//#include <functional>
//#include <type_traits>
//#include <cassert>

#ifndef PORT_H
#define PORT_H

#include <any>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>
#include <refl.hpp>

enum class PortRole { Master, Slave };

class Port {
public:
    // 构造函数
    Port() = default;
    explicit Port(PortRole role) : role_(role) {}

    // 绑定到另一个端口
    void bind(Port* peer) {
        peer_ = peer;
    }

    // 设置角色
    void setRole(PortRole role) {
        role_ = role;
    }

    // 获取角色
    PortRole getRole() const {
        return role_;
    }

    // 模板函数：发送数据
    template <typename T>
    void send(const T& data) {
        sendData(std::any(data));
    }

    // 模板函数：接收数据
    template <typename T>
    T receive() {
        if (hasData()) {
            T data = std::any_cast<T>(receiveData());
            return data;
        }
        throw std::runtime_error("No data available to receive");
    }

    // 检查是否有数据可以接收
    bool hasData() const {
        return !dataQueue_.empty();
    }

    // 克隆函数
    std::unique_ptr<Port> clone() const {
        return std::make_unique<Port>(*this);
    }

    // 添加观察者
    void addObserver(std::function<void()> observer) {
        observers_.push_back(observer);
    }

    // 通知观察者
    void notifyObservers() {
        for (const auto& observer : observers_) {
            observer();
        }
    }

    // 存储数据
    void sendData(const std::any& data) {
        if (peer_ && role_ == PortRole::Master) {
            peer_->sendData(data);
        } else {
            dataQueue_.push(data);
        }
    }

    // 获取数据
    std::any receiveData() {
        if (peer_ && role_ == PortRole::Slave) {
            if (peer_->hasData()) {
                return peer_->receiveData();
            }
        } else if (!dataQueue_.empty()) {
            std::any data = dataQueue_.front();
            dataQueue_.pop();
            return data;
        }
        throw std::runtime_error("No data available to receive");
    }

    // 对端指针
    Port* peer_ = nullptr;

    // 角色
    PortRole role_ = PortRole::Master;

    // 数据队列
    std::queue<std::any> dataQueue_;

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
    field(role_)
    )


#endif // PORT_H
