#pragma once

#include <modbus/modbus.h>
#include <string>
#include "inverter_data.h"
#include "config.h"
#include "threadsafequeue.h"

class ModbusReader {
public:
    ModbusReader(const Config& config, ThreadSafeQueue<InverterData>& queue);
    ~ModbusReader();
    
    bool connect();
    void disconnect();
    bool isConnected() const { return connected_; }
    
    bool readInverterData(InverterData& data);
    
private:
    bool readRegister(int address, uint16_t& value);
    
    Config config_;
    modbus_t* ctx_;
    bool connected_;
    ThreadSafeQueue<InverterData>& dataQueue_;
};
