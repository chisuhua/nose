#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>


class PathUtils {
public:
    static std::vector<std::string> split(const std::string& path) {
        std::istringstream iss(path);
        std::vector<std::string> parts;
        std::string part;
        while (std::getline(iss, part, '/')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }
        return parts;
    }

    static std::string join(const std::vector<std::string>& parts) {
        std::ostringstream oss;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i != 0) {
                oss << "/";
            }
            oss << parts[i];
        }
        return oss.str();
    }

    static std::string get_parent(const std::string& path) {
        auto pos = path.find_last_of('/');
        if (pos == std::string::npos) return "/";
        return path.substr(0, pos);
    }

    static std::string parseTypeName(const std::string& part) {
        auto pos = part.find('<');
        if (pos != std::string::npos) {
            std::string typeName = part.substr(0, pos);
            std::string templateArg = part.substr(pos + 1, part.find('>') - pos - 1);
            return  typeName + "<" + templateArg + ">";
        } else {
            return part;
        }
    }
};

