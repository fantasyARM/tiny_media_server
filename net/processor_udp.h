#pragma once
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "processor.h"
namespace tiny {
static const std::string defaultip = "0.0.0.0";

using UdpPacket = struct UdpPacket_tag {
    UdpPacket_tag(std::vector<uint8_t> &&buf, const int len, const struct sockaddr_in &from_addr, const std::string &in_ip, const short in_port) {
        in_addr_ = from_addr;
        addr_.sin_addr.s_addr = inet_addr(in_ip.c_str());
	addr_.sin_port = htons(in_port);
        d_ = buf;
        len_ = len;
    }

    std::vector<uint8_t> d_;
    int len_{0};
    struct sockaddr_in addr_{0};
    struct sockaddr_in in_addr_{0};
};

class ProcessorUdp : public Processor
{
public:
    static constexpr int MAX_UDP_PKT = 65535;
    explicit ProcessorUdp() = default;
    
    virtual ~ProcessorUdp() = default;
    
    int Init(const short port, const std::string& addr = defaultip);
    
    int ProcessEvent();
    
    int HandleRead();
    
    int HandleWrite();
    
    int HandleStop();

    int HandleTimeout();

    virtual int ConsumeData(const std::shared_ptr<UdpPacket> &d);
private:
    int listenPort_{0};
    std::string listenAddr_;
};
}
