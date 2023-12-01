#pragma once
#include <memory>
#include "processor.h"

namespace tiny {
class Event
{
public:
    using NotifyCall = std::function<void(int, const std::shared_ptr<Event>& e)>;
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
    
    void SetNotify(NotifyCall &&n) {
        notify_ = n;
    }

    void SetInsert(NotifyCall &&n) {
        insert_ = n;
    }

    NotifyCall notify_;
    NotifyCall insert_;
public:
    int type_{0};
    Processor* handler_{nullptr}; //used for epoll data.ptr 
    int readTimes_{0};
    int writeTimes_{0};
    int readBytes_{0};
    int writeBytes_{0};
    int timeOut_{0}; //ms 
};
}
