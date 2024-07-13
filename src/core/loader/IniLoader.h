#pragma once
#include "TypeManager.h"
#include "Tree.h"
#include <regex>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

class IniLoader {
public:
    explicit IniLoader(TypeManager& typeManager) : typeManager_(typeManager) {}

    void load(const std::string& filename, Tree& tree) {
        // 假设实现载入INI文件内容，然后调用createPath函数
        // 这是一个读取文件并解析每一行的示例
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue; // Skip comments and empty lines
            }
            
            auto delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string path = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                createPath(tree, path, value);
            }
        }
    }

    void createPath(Tree &tree, const std::string &path, const std::string &value) {
        auto parts = splitPath(path);
        auto currentNode = (path.front() == '/') ? tree.getRoot() : tree.getCurrent();
        std::string typeName;
        bool isObject = false;

        for (size_t i = 0; i < parts.size(); ++i) {
            auto part = parts[i];
            auto pos = part.find(':');
            if (pos != std::string::npos) {
                typeName = part.substr(pos + 1);
                part = part.substr(0, pos);
                isObject = true;
            }

            if (i == parts.size() - 1 && !typeName.empty()) {
                currentNode->setType(typeName);
                currentNode->setProperty(typeName, part, parseValueIfNeeded(currentNode, part, value));
            } else {
                auto child = currentNode->getChild(part);
                if (!child) {
                    child = std::make_shared<Node>(part);
                    currentNode->addChild(child);
                }
                currentNode = child;

                if (isObject) {
                    currentNode->setType(typeName);
                    isObject = false;
                }
            }
        }
    }

private:
    TypeManager& typeManager_;

    std::any parseValueIfNeeded(std::shared_ptr<Node> currentNode, const std::string& memberName, const std::string& value) {
        // 遍历 TypeManager 中注册的所有类型，处理 :<Type> 的值
        auto registeredTypes = typeManager_.getTypeConstructors();

        for (const auto& [typeName, _] : registeredTypes) {
            if (memberName.find(typeName) != std::string::npos) {
                std::regex re(R"(\{([^\}]*)\})");
                std::smatch match;
                if (std::regex_search(value, match, re)) {
                    std::string domainList = match[1].str();
                    // std::vector<std::shared_ptr<void>> objects;
                    std::vector<std::any> objects;

                    std::istringstream stream(domainList);
                    std::string domain;
                    while (std::getline(stream, domain, ',')) {
                        domain = trim(domain);
                        if (domain.find(":" + typeName) != std::string::npos) {
                            domain = domain.substr(0, domain.find(":" + typeName));
                            if (domain.find('*') != std::string::npos) {
                                auto basePath = domain.substr(0, domain.size() - 1);
                                auto baseNode = currentNode->findNode(basePath);
                                if (baseNode) {
                                    for (const auto& [key, child] : baseNode->getChildren()) {
                                        auto object = child->getObject(typeName);
                                        if (object.has_value()) {
                                            objects.push_back(object);
                                        }
                                    }
                                }
                            } else {
                                auto node = currentNode->findNode(domain);
                                if (node) {
                                    auto object = node->getObject(typeName);
                                    if (object.has_value()) {
                                        objects.push_back(object);
                                    }
                                }
                            }
                        }
                    }
                    // return std::make_any<std::vector<std::shared_ptr<void>>>(objects);
                    return objects;
                }
            }
        }

        // 其他普通情况处理
        const auto& type = typeManager_.getTypeConstructors().at(currentNode->getType());
        refl::runtime::for_each(type.members, [&](auto member) {
            if (member.name.str() == memberName) {
                using member_type = typename decltype(member)::value_type;
                const auto& prop = refl::descriptor::get_attribute<IoProperty<>>(member);
                currentNode->setProperty(type.name().str(), memberName, TypeManager::parseValue<member_type>(prop, value));
            }
        });

        return value;
    }

    std::string trim(const std::string& str) {
        const char* whitespace = " \t\n\r\f\v";
        size_t start = str.find_first_not_of(whitespace);
        size_t end = str.find_last_not_of(whitespace);
        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    // 分割路径
    std::vector<std::string> splitPath(const std::string& path) {
        std::vector<std::string> result;
        std::stringstream ss(path);
        std::string item;

        while (std::getline(ss, item, '/')) {
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        return result;
    }
};

