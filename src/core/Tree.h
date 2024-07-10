
// Tree.hpp
#include "Node.hpp"
#include <memory>

class Tree {
public:
    Tree() : root_(std::make_shared<Node>("root")) {}

    std::shared_ptr<Node> getRoot() const { return root_; }
    std::shared_ptr<Node> getCurrent() const { return current_; }
    void setCurrent(std::shared_ptr<Node> current) { current_ = current; }

    std::shared_ptr<Node> findNode(const std::string& path) const {
        auto parts = splitPath(path);
        auto currentNode = root_;

        for (const auto& part : parts) {
            auto child = currentNode->getChild(part);
            if (!child) {
                return nullptr;
            }
            currentNode = child;
        }
        return currentNode;
    }

private:
    std::shared_ptr<Node> root_;
    std::shared_ptr<Node> current_;

    std::vector<std::string> splitPath(const std::string& path) const {
        std::vector<std::string> parts;
        std::string item;
        std::stringstream ss(path);
        while (std::getline(ss, item, '/')) {
            if (!item.empty()) {
                parts.push_back(item);
            }
        }
        return parts;
    }
};
