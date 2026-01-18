#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

struct RegisterDef {
    int address;
    std::string name;
    double scale;
    bool isSigned;
    std::string dataType;
};

struct Config {
    std::string modbusPort = "/dev/ttyUSB0";
    int modbusBaudrate = 19200;
    char modbusParity = 'N';
    int modbusDataBits = 8;
    int modbusStopBits = 1;
    int modbusDeviceId = 4;
    std::string udpHost = "127.0.0.1";
    int udpPort = 9999;
    int pollInterval = 1000;
    std::vector<RegisterDef> registers;
    
    bool loadFromFile(const std::string& path);
};
