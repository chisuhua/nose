
// Visitor.hpp
#include <memory>
#include <string>

class Node;

template<typename T>
class Visitor {
public:
    virtual void visit(Node& node) = 0;
    virtual void visitObject(const std::shared_ptr<void>&, const std::string&) = 0;
};
