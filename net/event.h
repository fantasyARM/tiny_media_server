#pragma once
#include <memory>
#include "common.h"

namespace tiny {
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

class Event;
class Processor
{
public:
    using NotifyCall = std::function<void(int, const std::shared_ptr<Event>& e)>;
    Processor() = default;
    virtual ~Processor() {
        CLOSE(fd_);
    }

    virtual int HandleRead() {
        perror("this function read need defined\n");
        abort();
    }

    virtual int HandleWrite(const std::shared_ptr<UdpPacket> &pkt) {
        perror("this function write need defined\n");
        abort();
    }

    virtual int HandleSyncWrite(const std::shared_ptr<UdpPacket> &pkt) {
        perror("this function sync write need defined\n");
        abort();
    }

    virtual int HandleStop() {
        perror("this function stop need defined\n");
        abort();
    }

    virtual int HandleTimeout() {
        perror("this function timeout need defined\n");
        abort();
    }

    const int GetFd() {
        return fd_;
    }
    
    void SetNotify(NotifyCall &&n) {
        notify_ = n;
    }

    void SetInsert(NotifyCall &&n) {
        insert_ = n;
    }
public:
    NotifyCall notify_;
    NotifyCall insert_;
    int fd_{-1};
};

class Event
{
public:
    Event(const int t, Processor* p)
    : type_(t), handler_(p) {
    }
    
    virtual ~Event() = default;
    
    int GetFd() {
        if (handler_)
            return handler_->GetFd();
        else
            return -1;
    }

    int GetType() {
        return type_;
    }

    Processor* Handle() {
        return handler_;
    }
    
    void SetWritePkt(const std::shared_ptr<UdpPacket> &p) {
        w_pkt_ = p;
    }

    std::shared_ptr<UdpPacket> GetPkt() {
        return w_pkt_;
    }
public:
    int type_{0};
    Processor* handler_{nullptr}; //used for epoll data.ptr 
    int readTimes_{0};
    int writeTimes_{0};
    int readBytes_{0};
    int writeBytes_{0};
    int timeOut_{0}; //ms 
    std::shared_ptr<UdpPacket> w_pkt_; 
};
}
