#include "config.h"
#include <sstream>

bool Config::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << path << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Minimal JSON parser for our simple structure
    auto getValue = [&](const std::string& key) -> std::string {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";
        pos = content.find(":", pos);
        if (pos == std::string::npos) return "";
        pos = content.find_first_not_of(" \t\n\r:", pos + 1);
        if (pos == std::string::npos) return "";
        
        if (content[pos] == '"') {
            size_t end = content.find('"', pos + 1);
            return content.substr(pos + 1, end - pos - 1);
        } else {
            size_t end = content.find_first_of(",}\n", pos);
            return content.substr(pos, end - pos);
        }
    };
    
    modbusPort = getValue("port");
    if (modbusPort.empty()) modbusPort = "/dev/ttyUSB0";
    
    std::string val = getValue("baudrate");
    if (!val.empty()) modbusBaudrate = std::stoi(val);
    
    val = getValue("parity");
    if (!val.empty()) modbusParity = val[0];
    
    val = getValue("dataBits");
    if (!val.empty()) modbusDataBits = std::stoi(val);
    
    val = getValue("stopBits");
    if (!val.empty()) modbusStopBits = std::stoi(val);
    
    val = getValue("deviceId");
    if (!val.empty()) modbusDeviceId = std::stoi(val);
    
    udpHost = getValue("host");
    if (udpHost.empty()) udpHost = "127.0.0.1";
    
    val = getValue("port");
    size_t udpPos = content.find("\"udp\"");
    if (udpPos != std::string::npos) {
        size_t portPos = content.find("\"port\"", udpPos);
        if (portPos != std::string::npos) {
            portPos = content.find(":", portPos);
            portPos = content.find_first_of("0123456789", portPos);
            if (portPos != std::string::npos) {
                size_t end = content.find_first_of(",}\n", portPos);
                udpPort = std::stoi(content.substr(portPos, end - portPos));
            }
        }
    }
    
    val = getValue("pollInterval");
    if (!val.empty()) pollInterval = std::stoi(val);
    
    // Parse registers array
    size_t regStart = content.find("\"registers\"");
    if (regStart != std::string::npos) {
        regStart = content.find("[", regStart);
        size_t regEnd = content.find("]", regStart);
        std::string regSection = content.substr(regStart, regEnd - regStart);
        
        size_t pos = 0;
        while ((pos = regSection.find("{", pos)) != std::string::npos) {
            size_t objEnd = regSection.find("}", pos);
            std::string obj = regSection.substr(pos, objEnd - pos);
            
            RegisterDef reg;
            reg.scale = 1.0;
            reg.isSigned = false;
            reg.dataType = "UInt16";
            
            size_t addrPos = obj.find("\"address\"");
            if (addrPos != std::string::npos) {
                addrPos = obj.find(":", addrPos);
                addrPos = obj.find_first_of("0123456789", addrPos);
                size_t end = obj.find_first_of(",}", addrPos);
                reg.address = std::stoi(obj.substr(addrPos, end - addrPos));
            }
            
            size_t namePos = obj.find("\"name\"");
            if (namePos != std::string::npos) {
                namePos = obj.find("\"", namePos + 6);
                size_t end = obj.find("\"", namePos + 1);
                reg.name = obj.substr(namePos + 1, end - namePos - 1);
            }
            
            size_t scalePos = obj.find("\"scale\"");
            if (scalePos != std::string::npos) {
                scalePos = obj.find(":", scalePos);
                scalePos = obj.find_first_of("0123456789.-", scalePos);
                size_t end = obj.find_first_of(",}", scalePos);
                reg.scale = std::stod(obj.substr(scalePos, end - scalePos));
            }
            
            size_t signedPos = obj.find("\"isSigned\"");
            if (signedPos != std::string::npos) {
                signedPos = obj.find(":", signedPos);
                signedPos = obj.find_first_not_of(" \t\n\r:", signedPos + 1);
                reg.isSigned = (obj.substr(signedPos, 4) == "true");
            }
            
            size_t typePos = obj.find("\"dataType\"");
            if (typePos != std::string::npos) {
                typePos = obj.find("\"", typePos + 10);
                size_t end = obj.find("\"", typePos + 1);
                reg.dataType = obj.substr(typePos + 1, end - typePos - 1);
            }
            
            registers.push_back(reg);
            pos = objEnd;
        }
    }
    
    std::cout << "Loaded " << registers.size() << " registers from config" << std::endl;
    
    return true;
}
