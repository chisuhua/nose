#pragma once
#include <memory>
#include "PathUtils.h"
#include "Entity.h"

class Tree {
public:
    Tree() {
        root_ = std::make_shared<Entity>("/");
        current_ = root_;
    }

    std::shared_ptr<Entity> getRoot() const { return root_; }
    std::shared_ptr<Entity> getCurrent() const { return current_; }

    void setCurrent(std::shared_ptr<Entity> current) { current_ = current; }

    void changeCurrent(const std::string& path) {
        std::shared_ptr<Entity> start_entity = (path.front() == '/') ? root_ : current_;
        current_ = traverseToEntity(start_entity, path);
        if (current_) {
            throw std::runtime_error("Path not found:" + path);
        }
    }

    std::shared_ptr<Entity> findEntity(const std::string& path) const {
        auto parts = PathUtils::split(path);
        auto current_entity = root_;

        for (const auto& part : parts) {
            auto child = current_entity->getChild(part);
            if (!child) {
                return nullptr;
            }
            current_entity = child;
        }
        return current_entity;
    }

    void accept(Visitor<void>& visitor, const std::string& path = "") {
        std::shared_ptr<Entity> start_entity = (path.empty() || path.front() == '/') ? root_ : current_;
        auto target_entity = traverseToEntity(start_entity, path);
        if (!target_entity) {
            throw std::runtime_error("Path not found: " + path);
        }
        target_entity->accept(visitor);
    }


private:
    std::shared_ptr<Entity> traverseToEntity(std::shared_ptr<Entity> start_entity, const std::string& path) {
        std::vector<std::string> parts = PathUtils::split(path);
        auto current = start_entity;

        for (const auto& part : parts) {
            if (part == "..") {
                current = current->getParent().lock();
                if (!current) {
                    throw std::runtime_error("No parent directory");
                }
            } else {
                auto child = current->getChild(part);
                if (!child) {
                    child = std::make_shared<Entity>(part);
                    current->addChild(child);
                }
                current = child;
            }
        }
        return current;
    }

    
    std::shared_ptr<Entity> root_;
    std::shared_ptr<Entity> current_;
};
