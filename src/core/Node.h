// Node.hpp
#include <map>
#include <string>
#include <memory>
#include <any>
#include <unordered_map>
#include <refl.hpp>

class Node {
public:
    using ElementProperties = std::unordered_map<std::string, std::map<std::string, std::any>>;

    explicit Node(const std::string& name) : name_(name) {}

    const std::string& getName() const { return name_; }
    std::shared_ptr<Node> getChild(const std::string& name) const {
        auto it = children_.find(name);
        if (it != children_.end()) {
            return it->second;
        }
        return nullptr;
    }
    void addChild(std::shared_ptr<Node> child) { children_[child->getName()] = child; }
    const std::map<std::string, std::shared_ptr<Node>>& getChildren() const { return children_; }
    const ElementProperties& getProperties() const { return properties_; }
    std::any getObject(const std::string& typeName) const {
        auto it = objects_.find(typeName);
        if (it != objects_.end()) {
            return it->second;
        }
        return nullptr;
    }
    void setObject(const std::string& typeName, std::shared_ptr<void> object) { objects_[typeName] = object; }
    void setProperty(const std::string& typeName, const std::string& memberName, std::any value) { 
        properties_[typeName][memberName] = value;
    }

    void setType(const std::string& type) { type_ = type; }
    const std::string& getType() const { return type_; }

    std::shared_ptr<Node> findNode(const std::string& path) const {
        auto parts = splitPath(path);
        auto currentNode = const_cast<Node*>(this)->shared_from_this();

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
    std::string name_;
    std::string type_;
    std::map<std::string, std::shared_ptr<Node>> children_;
    ElementProperties properties_;
    std::unordered_map<std::string, std::shared_ptr<void>> objects_;

    static std::vector<std::string> splitPath(const std::string& path) {
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

