#pragma once
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <regex>
#include <map>
#include <stdexcept>
#include <iostream>
#include "Component.h"
#include "Channel.h"
#include "Registry.h"


class Clock {
public:
    uint64_t freq_;
    std::vector<std::shared_ptr<Component>> components_;
    std::vector<std::shared_ptr<Channel>> channels_;

    Clock() : freq_(0) {}

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
                freq_ = base * it->second;
            } else {
                freq_ = base;
            }
        } else {
            throw std::invalid_argument("Invalid frequency format");
        }
    }

    void tick() {
        for (auto& component : components) {
            component->tick();
        }

        for (auto& channel : channels) {
            channel->tick();
        }
    }
};

ValueType ParseComponents(const std::string& valueStr) {
    return ValueType(std::any(valueStr));
};

ValueType ParseChannels(const std::string& valueStr) {
    return ValueType(std::any(valueStr));
};

REFL_AUTO(
    type(Clock),
    field(freq_),
    field(components_, Property<ValueParser>(&ParseComponents)),
    field(channels_, Property<ValueParser>(&ParseChannels))
    )

