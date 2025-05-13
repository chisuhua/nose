#ifndef INILOADER_H
#define INILOADER_H

#include <fstream>
#include <stdexcept>
#include "Tree.h"
#include "TypeManager.h"
#include "StringIntern.h"
#include "PathUtils.h"

class IniLoader {
public:
    explicit IniLoader(TypeManager& typeManager) : typeManager_(typeManager) {}

    void load(const std::string& filename, Tree& tree) {
        // 假设实现载入INI文件内容，然后调用createPath函数
        // 这是一个读取文件并解析每一行的示例
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
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
                auto current_entity = createPath(tree, current_section);
                tree.setCurrent(current_entity);
            } else {
                auto delimiterPos = line.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = trim(line.substr(0, delimiterPos));
                    std::string value = trim(line.substr(delimiterPos + 1));
                    createPath(tree, key, value);
                } else {
                    createPath(tree, line);
                }
            }
        }
        file.close();
    }

    Path createPath(const Path &start_path, const std::string &subpath_str) {
        auto parts = PathUtils::split(subpath_str);
        auto current_path = start_path;


        StringRef current_type_name;

        for (size_t i = 0; i < parts.size(); ++i) {
            auto part = parts[i];
            current_path = Path(part, current_path);
        }
        return current_path;
    }

    Path createPath(Tree &tree, const std::string &path) {
        auto start_path = (path.front() == '/') ? tree.getRoot() : tree.getCurrent();
        return createPath(start_path, path);
    }

    Path createPath(Tree &tree, const std::string &path, const std::string &value_str ) {
        auto start_path = (path.front() == '/') ? tree.getRoot() : tree.getCurrent();
        auto this_path = createPath(start_path, path);
        bool is_object_path = this_path.isObjectPath();


        if (is_object_path) {
            auto obj_parts = this_path.getObjectPath();
            if (obj_parts.size() == 0) {
                // if ini line start with {....} or [...] , we use it to deseriarizle 
                this_path.setSerialize(value_str);
                this_path.deserialize();
            } else if (obj_parts.size() == 1) {
                // person = {...} , parse object member 
                ValueType value = typeManager_.parsePropertyValue(this_path.getTypeName(), obj_parts[0]->str(), value_str);
                this_path.setProperty(obj_parts[0]->str(), value);
            } else {
                throw std::runtime_error("init path currently don't support multilevel object path");
                // TODO tracing object_path
            }
            if (start_path.isObjectPath()) {
                ValueType value = typeManager_.parsePropertyValue(start_path.getTypeName(), this_path.getName(), this_path.getTypeName()->str());
                start_path.setProperty(this_path.getName(), value);
            }
        }
        return this_path;
    }

private:
    TypeManager& typeManager_;

    std::string trim(const std::string& str) {
        const char* whitespace = " \t\n\r\f\v";
        size_t start = str.find_first_not_of(whitespace);
        size_t end = str.find_last_not_of(whitespace);
        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }
};

#endif
