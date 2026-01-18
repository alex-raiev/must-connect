#pragma once

#include <string>
#include <atomic>
#include <thread>
#include "inverter_data.h"
#include "config.h"
#include "threadsafequeue.h"

class UdpSender {
public:
    UdpSender(const Config& config, ThreadSafeQueue<InverterData>& queue);
    ~UdpSender();
    
    bool init();
    bool sendData(const InverterData& data);
    void startSending();
    void stopSending();
    
private:
    void sendLoop();
    
    Config config_;
    ThreadSafeQueue<InverterData>& dataQueue_;
    int sockfd_;
    bool initialized_;
    std::atomic<bool> running_;
    std::thread sendThread_;
};
