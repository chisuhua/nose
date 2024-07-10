// Clock.hpp
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <regex>
#include <map>
#include <stdexcept>
#include <iostream>

class Component;

class Clock {
public:
    uint64_t freq;
    std::vector<std::shared_ptr<Component>> domains;
    std::vector<std::shared_ptr<Channel>> channels;

    Clock() : freq(0) {}

    void parseFrequency(const std::string& freqStr) {
        static const std::map<std::string, uint64_t> suffixes = {
            {"K", 1000},
            {"M", 1000000},
            {"G", 1000000000},
        };

        std::regex regex("([0-9]+)([KMG]?)");
        std::smatch match;

        if (std::regex_match(freqStr, match, regex)) {
            uint64_t base = std::stoull(match[1].str());
            std::string suffix = match[2].str();

            auto it = suffixes.find(suffix);
            if (it != suffixes.end()) {
                freq = base * it->second;
            } else {
                freq = base;
            }
        } else {
            throw std::invalid_argument("Invalid frequency format");
        }
    }

    void tick() {
        for (auto& component : domains) {
            component->clockUpdate();
        }

        for (auto& channel : channels) {
            channel->clockUpdate();
        }
    }
};

