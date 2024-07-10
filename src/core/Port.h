// Port.hpp
#include "IPort.hpp"

template<typename IOType>
class Port : public IPort {
public:
    Port() = default;

    const IOType& getIO() const {
        return io_;
    }

    std::any getIO() override {
        return io_;
    }

    void bind(std::shared_ptr<IPort> other) override {
        peer = other;
    }

    std::shared_ptr<IPort> clone() const override {
        auto clonedPort = std::make_shared<Port<IOType>>();
        clonedPort->io_ = this->io_;
        clonedPort->role = this->role;
        return clonedPort;
    }

private:
    IOType io_;
    std::function<void()> observer_;
};

