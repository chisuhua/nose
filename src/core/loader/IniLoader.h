#pragma once
#include <regex>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "Tree.h"
#include "TypeManager.h"
#include "PathUtils.h"

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
        std::string current_section;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue; // Skip comments and empty lines
            }

            if (line.front() == '[' && line.back() == ']') {
                current_section = line.substr(1, line.size() - 2);
                tree.changeCurrent(current_section);
            } else {
                auto delimiterPos = line.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = trim(line.substr(0, delimiterPos));
                    std::string value = trim(line.substr(delimiterPos + 1));
                    createPath(tree, current_section + "/" + key, value);
                } else {
                    createPath(tree, current_section + "/" + line);
                }
            }
        }
        file.close();
    }

    void createPath(Tree &tree, const std::string &path, const std::string &value_str = "") {
        auto parts = PathUtils::split(path);
        auto current_node = (path.front() == '/') ? tree.getRoot() : tree.getCurrent();
        std::string type_name;
        bool is_object = false;

        for (size_t i = 0; i < parts.size(); ++i) {
            auto part = parts[i];
            auto pos = part.find(':');
            if (pos != std::string::npos) {
                type_name = PathUtils::parseTypeName(part.substr(pos + 1));
                part = part.substr(0, pos);
                is_object = true;
            }

            if (is_object) {
                if (i == parts.size() - 1) {
                    ValueType value = typeManager_.parsePropertyValue(type_name, part, value_str);
                    current_node->setProperty(type_name, part, value);
                }
            } else {
                auto child = current_node->getChild(part);
                if (!child) {
                    child = std::make_shared<Node>(part);
                    current_node->addChild(child);
                }
                current_node = child;
            }
        }
    }

private:
    TypeManager& typeManager_;

    std::string trim(const std::string& str) {
        const char* whitespace = " \t\n\r\f\v";
        size_t start = str.find_first_not_of(whitespace);
        size_t end = str.find_last_not_of(whitespace);
        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    //std::any parseValueIfNeeded(std::shared_ptr<Node> current_node, const std::string& memberName, const std::string& value) {
        //// 遍历 TypeManager 中注册的所有类型，处理 :<Type> 的值
        //auto registeredTypes = typeManager_.getTypeConstructors();

        //for (const auto& [type_name, _] : registeredTypes) {
            //if (memberName.find(type_name) != std::string::npos) {
                //std::regex re(R"(\{([^\}]*)\})");
                //std::smatch match;
                //if (std::regex_search(value, match, re)) {
                    //std::string domainList = match[1].str();
                    //// std::vector<std::shared_ptr<void>> objects;
                    //std::vector<std::any> objects;

                    //std::istringstream stream(domainList);
                    //std::string domain;
                    //while (std::getline(stream, domain, ',')) {
                        //domain = trim(domain);
                        //if (domain.find(":" + type_name) != std::string::npos) {
                            //domain = domain.substr(0, domain.find(":" + type_name));
                            //if (domain.find('*') != std::string::npos) {
                                //auto basePath = domain.substr(0, domain.size() - 1);
                                //auto baseNode = current_node->findNode(basePath);
                                //if (baseNode) {
                                    //for (const auto& [key, child] : baseNode->getChildren()) {
                                        //auto object = child->getObject(type_name);
                                        //if (object.has_value()) {
                                            //objects.push_back(object);
                                        //}
                                    //}
                                //}
                            //} else {
                                //auto node = current_node->findNode(domain);
                                //if (node) {
                                    //auto object = node->getObject(type_name);
                                    //if (object.has_value()) {
                                        //objects.push_back(object);
                                    //}
                                //}
                            //}
                        //}
                    //}
                    //// return std::make_any<std::vector<std::shared_ptr<void>>>(objects);
                    //return objects;
                //}
            //}
        //}

        //// 其他普通情况处理
        //const auto& type = typeManager_.getTypeConstructors().at(current_node->getType());
        //refl::runtime::for_each(type.members, [&](auto member) {
            //if (member.name.str() == memberName) {
                //using member_type = typename decltype(member)::value_type;
                //const auto& prop = refl::descriptor::get_attribute<IoProperty<>>(member);
                //current_node->setProperty(type.name().str(), memberName, TypeManager::parseValue<member_type>(prop, value));
            //}
        //});

        //return value;
    //}

};

