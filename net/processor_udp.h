#pragma once
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "event.h"
namespace tiny {
static const std::string defaultip = "0.0.0.0";

class ProcessorUdp : public Processor
{
public:
    static constexpr int MAX_UDP_PKT = 65535;
    explicit ProcessorUdp() = default;
    
    virtual ~ProcessorUdp() = default;
    
    int Init(const short port, const std::string& addr = defaultip);
    
    int ProcessEvent();
    
    int HandleRead();
    
    int HandleWrite(const std::shared_ptr<UdpPacket> &pkt);
   
    int HandleSyncWrite(const std::shared_ptr<UdpPacket> &pkt);

    int HandleStop();

    int HandleTimeout();

    virtual int ConsumeData(const std::shared_ptr<UdpPacket> &d);
private:
    int listenPort_{0};
    std::string listenAddr_;
};
}
