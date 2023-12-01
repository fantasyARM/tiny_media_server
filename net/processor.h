#pragma once
#include <unistd.h>
#include "common.h"
namespace tiny {
class Processor
{
public:
    Processor() = default;

    virtual ~Processor() {
        if (fd_) {
	    close(fd_);
	}
	fd_ = -1;
    }

    virtual int HandleRead() {
        perror("this function read need defined\n");
        abort();
    }

    virtual int HandleWrite() {
        perror("this function write need defined\n");
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
    
public:
    int fd_{-1};
};
}
