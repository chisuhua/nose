#ifndef IPORT_H
#define IPORT_H

#include <any>
#include <queue>
#include <memory>
#include <functional>

enum class PortRole { Master, Slave };

class IPort {
public:
    virtual ~IPort() = default;

    // 绑定到另一个端口
    virtual void bind(IPort* peer) = 0;

    // 设置角色
    virtual void setRole(PortRole role) = 0;

    // 获取角色
    virtual PortRole getRole() const = 0;

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

    // 添加观察者
    virtual void addObserver(std::function<void()> observer) = 0;

    // 通知观察者
    virtual void notifyObservers() = 0;

    // 存储数据的虚函数
    virtual void sendData(const std::any& data) = 0;

    // 获取数据的虚函数
    virtual std::any receiveData() = 0;

    // 检查是否有数据的虚函数
    virtual bool hasDataImpl() const = 0;

    // 数据队列
    std::queue<std::any> dataQueue_;
};

// 具体实现
class Port : public IPort {
public:
    // 默认构造函数
    Port() = default;

    // 构造函数
    explicit Port(PortRole role) : role_(role) {}

    // 绑定到另一个端口
    void bind(IPort* peer) override {
        peer_ = peer;
    }

    // 设置角色
    void setRole(PortRole role) override {
        role_ = role;
    }

    // 获取角色
    PortRole getRole() const override {
        return role_;
    }

    // 克隆函数
    std::unique_ptr<IPort> clone() const override {
        return std::make_unique<Port>(*this);
    }

    // 检查是否有数据可以接收
    bool hasData() const override {
        return hasDataImpl();
    }

    // 添加观察者
    void addObserver(std::function<void()> observer) override {
        observers_.push_back(observer);
    }

    // 通知观察者
    void notifyObservers() override {
        for (const auto& observer : observers_) {
            observer();
        }
    }

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

    // 对端指针
    IPort* peer_ = nullptr;

    // 角色
    PortRole role_ = PortRole::Master;

    // 观察者列表
    std::vector<std::function<void()>> observers_;

    // 删除拷贝构造函数和赋值运算符
    Port(const Port&) = delete;
    Port& operator=(const Port&) = delete;
};


REFL_AUTO(
    type(Port, bases<IPort>),
    field(role_)
    )

#endif // IPORT_H
