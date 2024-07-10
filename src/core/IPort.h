// IPort.hpp
#include <memory>
#include <functional>

class IPort {
public:
    virtual ~IPort() = default;
    Role role;
    std::shared_ptr<IPort> peer;

    virtual std::any getIO() = 0;
    virtual void bind(std::shared_ptr<IPort> other) = 0;
    virtual std::shared_ptr<IPort> clone() const = 0;
};

