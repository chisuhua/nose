#ifndef CLOCK_H
#define CLOCK_H
#include <vector>
#include <string>
#include <memory>
#include <regex>
#include <map>
#include <stdexcept>
#include "Component.h"
#include "Channel.h"

class Clock ;

struct ClockData {
    EntityHashType entity_hash;
    uint64_t freq_;
    std::vector<ObjPtr<Component>> components_;
    std::vector<ObjPtr<Channel>> channels_;
};

class Clock {
public:
    using GenericType = std::shared_ptr<ClockData>;
    GenericType generic_;

    explicit Clock(GenericType generic) 
        : generic_(generic) {
            if (freq_ != 0) { generic->freq_ = freq_;}
        }

    static uint64_t parseFrequency(const std::string& freqStr) {
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
                return base * it->second;
            } else {
                return base;
            }
        } else {
            throw std::invalid_argument("Invalid frequency format");
        }
    }

    void tick() {
        for (auto& component : generic_->components_) {
            component->tick();
        }

        for (auto& channel : generic_->channels_) {
            channel->tick();
        }
    }

    virtual void bind() {
        for (auto& component : components_) {
            generic_->components_.push_back(Path::make(component).make_object<Component>());
        }
        for (auto& channel : channels_) {
            generic_->channels_.push_back(Path::make(channel).make_object<Channel>());
        }
    }

    uint64_t freq_ {0};
    std::vector<std::string> components_;
    std::vector<std::string> channels_;
};

//ValueType ParseComponents(const std::string& valueStr) {
    //return ValueType(std::any(valueStr));
//};

//ValueType ParseChannels(const std::string& valueStr) {
    //return ValueType(std::any(valueStr));
//};

REFL_AUTO(
    type(Clock),
    field(freq_, Property(&Clock::parseFrequency)),
    field(components_),
    field(channels_)
    )
REGISTER_OBJECT(Clock)

#endif
