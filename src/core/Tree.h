#ifndef TREE_H
#define TREE_H
#include "PathUtils.h"
#include "Path.h"

class Tree {
public:
    Tree() 
        : root_(Path("/", ""))
        , current_(root_)
    {}

    Tree(std::string name)
        : root_(Path(name, ""))
        , current_(root_)
    {
    }

    const Path& getRoot() const { return root_; }
    const Path& getCurrent() const { return current_; }

    void setCurrent(const Path& current) { current_ = current; }

    void changeCurrent(const std::string& path) {
        Path start_entity = (path.front() == '/') ? root_ : current_;
        current_ = traverseToEntity(start_entity, path);
        if (!current_) {
            throw std::runtime_error("Path not found:" + path);
        }
    }

    Path findEntity(const std::string& path) const {
        auto parts = PathUtils::split(path);
        auto current_entity = root_;

        for (const auto& part : parts) {
            auto child = Path(part, current_entity);
            current_entity = child;
        }
        return current_entity;
    }

    template<typename T = void>
    void accept(Visitor<T>& visitor, const std::string& path = "") {
        Path start_entity = (path.empty() || path.front() == '/') ? root_ : current_;
        auto target_entity = traverseToEntity(start_entity, path);
        if (!target_entity) {
            throw std::runtime_error("Path not found: " + path);
        }
        target_entity.accept(visitor);
    }


private:
    Path traverseToEntity(Path start_entity, const std::string& path) {
        std::vector<std::string> parts = PathUtils::split(path);
        auto current = start_entity;

        for (const auto& part : parts) {
            if (part == "..") {
                current = current.getParent();
                if (!current) {
                    throw std::runtime_error("No parent directory");
                }
            } else {
                auto child = Path(part, current);
                current = child;
            }
        }
        return current;
    }

    
    Path root_;
    Path current_;
};
#endif
