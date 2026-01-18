#pragma once

#include <cstdint>
#include <ctime>
#include <map>
#include <string>

struct InverterData {
    uint64_t timestamp;
    std::map<std::string, double> values;
    
    InverterData() : timestamp(time(nullptr)) {}
};
