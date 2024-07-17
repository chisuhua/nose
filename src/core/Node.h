#pragma once
#include <map>
#include <string>
#include <memory>
#include <any>
#include <unordered_map>
#include <refl.hpp>
#include "Property.h"
#include "Visitor.h"
#include "PathUtils.h"

class Node : public std::enable_shared_from_this<Node> {
public:
    explicit Node(const std::string& name) : name_(name) {}

    const std::string& getName() const { return name_; }

    void addChild(std::shared_ptr<Node>& child) { 
        children_[child->getName()] = child; 
        child->setParent(const_cast<Node*>(this)->shared_from_this());
    }

    std::shared_ptr<Node> getChild(const std::string& name) const {
        auto it = children_.find(name);
        if (it != children_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void setParent(const std::shared_ptr<Node>& parent) { parent_ = parent; }
    std::weak_ptr<Node> getParent() { return parent_; }

    void setObject(const std::string& type_name, std::shared_ptr<void> object) { objects_[type_name] = object; }
    std::shared_ptr<void> getObject(const std::string& type_name) const {
        auto it = objects_.find(type_name);
        return (it != objects_.end()) ? it->second : nullptr;
    }

    const auto& getChildren() const { return children_; }

    std::unordered_map<std::string, ElementProperties> getProperties() const { 
        return properties_;
    }

    ValueType getProperty(const std::string& type_name, const std::string& member_name) const { 
        auto& type_property = properties_.at(type_name);
        return type_property.at(member_name); 
    }

    void setProperty(const std::string& type_name, const std::string& member_name, ValueType value) { 
        auto& type_property = properties_.at(type_name);
        type_property[member_name] = value;
    }

    void accept(Visitor<void>& visitor) {
        //for (const auto& [key, child] : children_) {
            //child->accept(visitor);
        //}
        //for (const auto& [key, object] : objects_) {
            //visitor.visitObject(object, key);
        //}
        visitor.visit(*this);
    }

    std::shared_ptr<Node> findNode(const std::string& path) const {
        auto parts = PathUtils::split(path);
        auto current_node = const_cast<Node*>(this)->shared_from_this();

        for (const auto& part : parts) {
            auto child = current_node->getChild(part);
            if (!child) {
                return nullptr;
            }
            current_node = child;
        }
        return current_node;
    }

private:
    std::string name_;
    std::string type_;
    std::unordered_map<std::string, std::shared_ptr<Node>> children_;
    std::unordered_map<std::string, std::shared_ptr<void>> objects_;
    std::unordered_map<std::string, ElementProperties> properties_;
    std::weak_ptr<Node> parent_;
};

