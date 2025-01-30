#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <string>
#include <vector>
#include <sstream>

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

    static std::string join(const std::vector<std::string>& parts, const std::string& delimiter = "/") {
        std::ostringstream oss;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i != 0) {
                oss << delimiter;
            }
            oss << parts[i];
        }
        return oss.str();
    }

    static std::string get_parent(const std::string& path) {
        if (path.empty()) return "/";
        size_t pos = path.find_last_of('/');
        if (pos == std::string::npos) return "/";
        if (pos == 0) return "/";
        return path.substr(0, pos);
    }

    static std::string get_leafname(const std::string& path) {
        if (path.empty()) return "";
        size_t pos = path.find_last_of('/');
        if (pos == std::string::npos) return path;
        return path.substr(pos + 1);
    }

    static std::string parseTypeName(const std::string& part) {
        auto pos = part.find('<');
        if (pos != std::string::npos) {
            std::string typeName = part.substr(0, pos);
            std::string templateArg = part.substr(pos + 1, part.find('>') - pos - 1);
            return typeName + "<" + templateArg + ">";
        } else {
            return part;
        }
    }
};

#endif // PATHUTILS_H
