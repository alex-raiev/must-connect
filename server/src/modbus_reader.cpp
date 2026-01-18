#include "modbus_reader.h"
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

ModbusReader::ModbusReader(const Config& config, ThreadSafeQueue<InverterData>& queue)
    : config_(config), ctx_(nullptr), connected_(false), dataQueue_(queue) {
}

ModbusReader::~ModbusReader() {
    disconnect();
}

bool ModbusReader::connect() {
    ctx_ = modbus_new_rtu(
        config_.modbusPort.c_str(),
        config_.modbusBaudrate,
        config_.modbusParity,
        config_.modbusDataBits,
        config_.modbusStopBits
    );
    
    if (ctx_ == nullptr) {
        std::cerr << "Failed to create Modbus context: " << modbus_strerror(errno) << std::endl;
        return false;
    }
    
    if (modbus_set_slave(ctx_, config_.modbusDeviceId) == -1) {
        std::cerr << "Failed to set slave ID: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx_);
        ctx_ = nullptr;
        return false;
    }
    
    if (modbus_connect(ctx_) == -1) {
        std::cerr << "Failed to connect: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx_);
        ctx_ = nullptr;
        return false;
    }
    
    connected_ = true;
    std::cout << "Connected to Modbus device on " << config_.modbusPort << std::endl;
    return true;
}

void ModbusReader::disconnect() {
    if (ctx_) {
        modbus_close(ctx_);
        modbus_free(ctx_);
        ctx_ = nullptr;
    }
    connected_ = false;
}

bool ModbusReader::readRegister(int address, uint16_t& value) {
    if (!connected_ || !ctx_) return false;
    
    uint16_t reg;
    int rc = modbus_read_registers(ctx_, address, 1, &reg);
    
    if (rc == -1) {
        std::cerr << "Failed to read register " << address << ": " << modbus_strerror(errno) << std::endl;
        return false;
    }
    
    value = reg;
    return true;
}

bool ModbusReader::readInverterData(InverterData& data) {
    if (!connected_) return false;
    
    data.timestamp = time(nullptr);
    data.values.clear();
    
    std::cout << "\n=== Inverter Data (" << config_.registers.size() << " registers) ===" << std::endl;
    
    int successCount = 0;
    for (const auto& reg : config_.registers) {
        uint16_t rawValue;
        if (readRegister(reg.address, rawValue)) {
            double scaledValue;
            if (reg.isSigned) {
                int16_t signedValue = static_cast<int16_t>(rawValue);
                scaledValue = signedValue * reg.scale;
            } else {
                scaledValue = rawValue * reg.scale;
            }
            
            data.values[reg.name] = scaledValue;
            std::cout << reg.name << ": " << scaledValue << std::endl;
            successCount++;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    
    std::cout << "Successfully read " << successCount << "/" << config_.registers.size() << " registers" << std::endl;
    
    if (successCount > 0) {
        dataQueue_.push(data);
        std::cout << "Data queued. Queue size: " << dataQueue_.size() << std::endl;
    }
    
    return successCount > 0;
}
