#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include "modbus_reader.h"
#include "udp_sender.h"
#include "config.h"
#include "threadsafequeue.h"

volatile bool running = true;

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received. Shutting down..." << std::endl;
    running = false;
}

void modbusReaderThread(ModbusReader& reader, int pollInterval) {
    while (running) {
        InverterData data;
        reader.readInverterData(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(pollInterval));
    }
}

int main(int argc, char* argv[]) {
    std::cout << "MUST Inverter Modbus Server v1.0.0" << std::endl;
    std::cout << "====================================" << std::endl;
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    Config config;
    std::string configPath = "config.json";
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            configPath = argv[++i];
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --config <file>       Config file path (default: config.json)" << std::endl;
            std::cout << "  --help                Show this help message" << std::endl;
            return 0;
        }
    }
    
    if (!config.loadFromFile(configPath)) {
        std::cerr << "Failed to load config from " << configPath << std::endl;
        return 1;
    }
    
    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  Modbus Port: " << config.modbusPort << std::endl;
    std::cout << "  Device ID: " << config.modbusDeviceId << std::endl;
    std::cout << "  UDP: " << config.udpHost << ":" << config.udpPort << std::endl;
    std::cout << "  Poll Interval: " << config.pollInterval << "ms" << std::endl;
    std::cout << "  Registers: " << config.registers.size() << std::endl;
    std::cout << std::endl;
    
    ThreadSafeQueue<InverterData> dataQueue;
    
    ModbusReader modbusReader(config, dataQueue);
    if (!modbusReader.connect()) {
        std::cerr << "Failed to connect to Modbus device" << std::endl;
        return 1;
    }
    
    UdpSender udpSender(config, dataQueue);
    if (!udpSender.init()) {
        std::cerr << "Failed to initialize UDP sender" << std::endl;
        return 1;
    }
    
    std::cout << "Server started. Press Ctrl+C to stop." << std::endl;
    
    std::thread readerThread(modbusReaderThread, std::ref(modbusReader), config.pollInterval);
    udpSender.startSending();
    
    readerThread.join();
    udpSender.stopSending();
    
    std::cout << "\nShutting down..." << std::endl;
    std::cout << "Queue size: " << dataQueue.size() << std::endl;
    std::cout << "Dropped packets: " << dataQueue.getDroppedCount() << std::endl;
    
    modbusReader.disconnect();
    
    return 0;
}
