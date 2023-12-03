#pragma once
#include "common.h"
#include "locked_queue.hpp"
#include "event.h"
namespace tiny {
#define NOTIFY(ret, e) \
{ \
    e->Handle()->notify_(ret, e); \
} void(0)
class Thread
{
public:
    explicit Thread() noexcept {
        th_ctx_ = std::make_shared<std::thread>([&](){
            ThreadLoop(); 			
        });
    }
    virtual ~Thread() = default;

    void SetStop() {
        stop_ = true;
    }
   
    void Join() {
        if (th_ctx_ && th_ctx_->joinable())
            th_ctx_->join();
    }

    int HandleEvent(const std::shared_ptr<Event> &e) {
        int ret = 0;
        if (e->GetType() & EPOLLIN) {
            if ((ret = e->Handle()->HandleRead()) == 0) {
	        NOTIFY(0, e);
	    } else {
	        NOTIFY(-1, e);
		RET(-1);
	    }
        }

        if (e->GetType() & EPOLLOUT) {//write data ok
            if ((ret = e->Handle()->HandleWrite(e->GetPkt())) == 0) {
	        NOTIFY(1, e);
	    } else {
	        NOTIFY(-1, e);
		RET(-2);
	    }
        }

        if (e->GetType() & EPOLLERR) {
            NOTIFY(-1, e); //process error
	}
	RET(0);
    }

    void ThreadLoop() {
        while (stop_) {
            if (que_.Empty()) {
                usleep(1000);
            }
            auto e = que_.BlockedGet();
            HandleEvent(e);
            usleep(1000);
        }
    }
public:
    LockedQueue<std::shared_ptr<Event> > que_;
    std::atomic<bool> stop_{false};    
    std::shared_ptr<std::thread> th_ctx_;
};
}
