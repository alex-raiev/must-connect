#include "udp_sender.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <chrono>

UdpSender::UdpSender(const Config& config, ThreadSafeQueue<InverterData>& queue)
    : config_(config), dataQueue_(queue), sockfd_(-1), initialized_(false), running_(false) {
}

UdpSender::~UdpSender() {
    stopSending();
    if (sockfd_ >= 0) {
        close(sockfd_);
    }
}

bool UdpSender::init() {
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0) {
        std::cerr << "Failed to create UDP socket" << std::endl;
        return false;
    }
    
    initialized_ = true;
    std::cout << "UDP sender initialized for " << config_.udpHost << ":" << config_.udpPort << std::endl;
    return true;
}

bool UdpSender::sendData(const InverterData& data) {
    if (!initialized_ || sockfd_ < 0) return false;
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(config_.udpPort);
    
    if (inet_pton(AF_INET, config_.udpHost.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << config_.udpHost << std::endl;
        return false;
    }
    
    // Simple text protocol: timestamp|name1:value1|name2:value2|...
    std::ostringstream oss;
    oss << data.timestamp;
    for (const auto& kv : data.values) {
        oss << "|" << kv.first << ":" << kv.second;
    }
    std::string payload = oss.str();
    
    ssize_t sent = sendto(sockfd_, payload.c_str(), payload.size(), 0,
                          (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    
    return sent > 0;
}

void UdpSender::startSending() {
    if (running_) return;
    running_ = true;
    sendThread_ = std::thread(&UdpSender::sendLoop, this);
}

void UdpSender::stopSending() {
    running_ = false;
    if (sendThread_.joinable()) {
        sendThread_.join();
    }
}

void UdpSender::sendLoop() {
    while (running_) {
        if (!dataQueue_.isEmpty()) {
            InverterData data = dataQueue_.pop();
            if (!sendData(data)) {
                std::cerr << "Failed to send UDP data" << std::endl;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}
