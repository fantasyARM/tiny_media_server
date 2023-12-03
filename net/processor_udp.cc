#include <string.h>
#include "defer.hpp"
#include "processor_udp.h"
#include "common.h"
using namespace tiny;

int ProcessorUdp::Init(const short port, const std::string& addr)
{
    int fd = -1;
    int ret = 0;
    defer [&]{
        if (ret != 0) {
	    CLOSE(fd);
	}        
    
    };
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("maybe fd not enough\n");
        RET(-1);
    }

    listenAddr_ = addr;
    listenPort_ = port;
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr.c_str());;
    
    if ((ret = bind(fd, (const struct sockaddr *)&server_addr, sizeof(server_addr))) < 0) {
        perror(" bind failed\n");
        RET(-2);
    }

    int oldflag = ::fcntl(fd, F_GETFL, 0);
    int newflag = oldflag | O_NONBLOCK;
    if ((ret = ::fcntl(fd, F_SETFL, newflag)) < 0) {
        perror(" set nonblock failed\n");
        RET(-3);
    }

    fd_ = fd;
    RET(0);
}

int ProcessorUdp::HandleRead()
{
    int ret = 0;
    std::vector<uint8_t> buf(MAX_UDP_PKT);
    struct sockaddr_in cli_addr = {0};
    socklen_t cli_addr_len = sizeof(cli_addr);
    if ((ret = ::recvfrom(fd_, &buf[0], MAX_UDP_PKT, 0, (struct sockaddr*)&cli_addr, &cli_addr_len)) < 0) {
        perror("recvfrom failed.");
	RET(-1);
    }

    auto pkt = std::make_shared<UdpPacket>(std::move(buf), ret, cli_addr, listenAddr_, listenPort_);
    if ((ret = ConsumeData(pkt) < 0)) {
        perror("process received data failed.");
	RET(-2);
    }

    RET(0);
}

int ProcessorUdp::HandleWrite(const std::shared_ptr<UdpPacket> &pkt)
{
    int ret = 0;
    RET(0);
}

int ProcessorUdp::HandleSyncWrite(const std::shared_ptr<UdpPacket> &pkt)
{
    int ret = 0;
    ret = sendto(fd_, &pkt->d_[0], pkt->len_, 0, (struct sockaddr*)&pkt->in_addr_, sizeof(pkt->in_addr_));
    printf("sendto ret ### %d\n", ret); 
    RET(0);
}

int ProcessorUdp::HandleStop()
{
    int ret = 0;

    RET(0);
}

int ProcessorUdp::HandleTimeout()
{
    int ret = 0;

    RET(0);
}

int ProcessorUdp::ConsumeData(const std::shared_ptr<UdpPacket> &d)
{
    char str[32] = {0};
    printf("here[%s:%d] recvfrom [%s:%d] %d bytes\n", inet_ntoa(d->addr_.sin_addr), ntohs(d->addr_.sin_port), inet_ntoa(d->in_addr_.sin_addr), ntohs(d->in_addr_.sin_port), d->len_);
    std::vector<uint8_t> rsp(64);
    std::string rspstr = "hello this is tiny server!";
    WriteString(&rsp[0], &rspstr[0], rspstr.length());
    auto tobe_write = std::make_shared<UdpPacket>(std::move(rsp), rspstr.length(), d->in_addr_, listenAddr_, listenPort_);
    return HandleSyncWrite(tobe_write);
}

