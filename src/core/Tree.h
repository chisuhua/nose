#pragma once
#include <memory>
#include "PathUtils.h"
#include "Node.h"

class Tree {
public:
    Tree() {
        root_ = std::make_shared<Node>("/");
        current_ = root_;
    }

    std::shared_ptr<Node> getRoot() const { return root_; }
    std::shared_ptr<Node> getCurrent() const { return current_; }

    void setCurrent(std::shared_ptr<Node> current) { current_ = current; }

    void changeCurrent(const std::string& path) {
        std::shared_ptr<Node> start_node = (path.front() == '/') ? root_ : current_;
        current_ = traverseToNode(start_node, path);
        if (current_) {
            throw std::runtime_error("Path not found:" + path);
        }
    }

    std::shared_ptr<Node> findNode(const std::string& path) const {
        auto parts = PathUtils::split(path);
        auto current_node = root_;

        for (const auto& part : parts) {
            auto child = current_node->getChild(part);
            if (!child) {
                return nullptr;
            }
            current_node = child;
        }
        return current_node;
    }

    void accept(Visitor<void>& visitor, const std::string& path = "") {
        std::shared_ptr<Node> start_node = (path.empty() || path.front() == '/') ? root_ : current_;
        auto target_node = traverseToNode(start_node, path);
        if (!target_node) {
            throw std::runtime_error("Path not found: " + path);
        }
        target_node->accept(visitor);
    }


private:
    std::shared_ptr<Node> traverseToNode(std::shared_ptr<Node> start_node, const std::string& path) {
        std::vector<std::string> parts = PathUtils::split(path);
        auto current = start_node;

        for (const auto& part : parts) {
            if (part == "..") {
                current = current->getParent().lock();
                if (!current) {
                    throw std::runtime_error("No parent directory");
                }
            } else {
                auto child = current->getChild(part);
                if (!child) {
                    child = std::make_shared<Node>(part);
                    current->addChild(child);
                }
                current = child;
            }
        }
        return current;
    }

    
    std::shared_ptr<Node> root_;
    std::shared_ptr<Node> current_;
};
