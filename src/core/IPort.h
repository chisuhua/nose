#ifndef IPORT_H
#define IPORT_H

#include <any>
#include <queue>
#include <memory>

enum class PortRole { Master, Slave };

class IPort {
public:
    virtual ~IPort() = default;

    // 绑定到另一个端口，并设置角色
    virtual void bind(IPort* peer, PortRole role) = 0;

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
    virtual bool hasData() const = 0;

    // 克隆函数
    virtual std::unique_ptr<IPort> clone() const = 0;

//protected:
    // 存储数据的虚函数
    virtual void sendData(const std::any& data) = 0;

    // 获取数据的虚函数
    virtual std::any receiveData() = 0;

    // 检查是否有数据的虚函数
    virtual bool hasDataImpl() const = 0;
};

// 具体实现
class Port : public IPort {
private:
    IPort* peer_ = nullptr;
    PortRole role_ = PortRole::Master;
    std::queue<std::any> dataQueue_;

public:
    // 绑定到另一个端口，并设置角色
    void bind(IPort* peer, PortRole role) override {
        peer_ = peer;
        role_ = role;
    }

    // 克隆函数
    std::unique_ptr<IPort> clone() const override {
        return std::make_unique<Port>(*this);
    }

    // 检查是否有数据可以接收
    bool hasData() const override {
        return hasDataImpl();
    }

//protected:
    // 存储数据
    void sendData(const std::any& data) override {
        if (peer_ && role_ == PortRole::Master) {
            peer_->sendData(data);
        } else {
            dataQueue_.push(data);
        }
    }

    // 获取数据
    std::any receiveData() override {
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

    // 检查是否有数据的虚函数
    bool hasDataImpl() const override {
        return !dataQueue_.empty();
    }
};


REFL_AUTO(
    type(Port, bases<IPort>),
    field(role_)
    )

#endif // IPORT_H
