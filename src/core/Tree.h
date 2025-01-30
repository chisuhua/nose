#pragma once
#include <memory>
#include "PathUtils.h"
#include "EntityIntern.h"

class Tree {
public:
    Tree() 
        : root_(EntityRef("/", ""))
        , current_(root_)
    {}

    Tree(std::string name)
        : root_(EntityRef(name, ""))
        , current_(root_)
    {
    }

    EntityRef getRoot() const { return root_; }
    EntityRef getCurrent() const { return current_; }

    void setCurrent(EntityRef current) { current_ = current; }

    void changeCurrent(const std::string& path) {
        EntityRef start_entity = (path.front() == '/') ? root_ : current_;
        current_ = traverseToEntity(start_entity, path);
        if (!current_) {
            throw std::runtime_error("Path not found:" + path);
        }
    }

    EntityRef findEntity(const std::string& path) const {
        auto parts = PathUtils::split(path);
        auto current_entity = root_;

        for (const auto& part : parts) {
            auto child = current_entity.getChild(part);
            if (!child) {
                return child;
            }
            current_entity = child;
        }
        return current_entity;
    }

    void accept(Visitor<void>& visitor, const std::string& path = "") {
        EntityRef start_entity = (path.empty() || path.front() == '/') ? root_ : current_;
        auto target_entity = traverseToEntity(start_entity, path);
        if (!target_entity) {
            throw std::runtime_error("Path not found: " + path);
        }
        target_entity.accept(visitor);
    }


private:
    EntityRef traverseToEntity(EntityRef start_entity, const std::string& path) {
        std::vector<std::string> parts = PathUtils::split(path);
        auto current = start_entity;

        for (const auto& part : parts) {
            if (part == "..") {
                current = current.getParent();
                if (!current) {
                    throw std::runtime_error("No parent directory");
                }
            } else {
                auto child = current.getChild(part);
                if (!child) {
                    return EntityRef();
                    //child = Entity(part, current.getPath());
                    //current.addChild(child);
                }
                current = child;
            }
        }
        return current;
    }

    
    EntityRef root_;
    EntityRef current_;
};
