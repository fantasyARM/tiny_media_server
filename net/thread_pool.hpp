#pragma once
#include "common.h"
#include "event.h"
#include "thread.h"
namespace tiny {
class ThreadPools
{
public:
    explicit ThreadPools(const int pools)
    : pools_count_(pools) {}
    virtual ~ThreadPools() = default;

    void InitPools() {
        for (int i = 0; i < pools_count_; i++) {
	    thread_list_.emplace_back(std::make_shared<Thread>());
	}
    }

    int HandleEvent(const std::shared_ptr<Event> &e) {
        int rid = Route(e->GetFd());
        printf("rid ## %d pools_count_ ### %d\n", rid, pools_count_);
	return thread_list_[rid]->HandleEvent(e);
    }

    int Route(const int fd) {
        return fd % pools_count_;
    }

    int Size() {
        return pools_count_;
    }

    int StopPools() {
        for (auto it: thread_list_) {
            it->SetStop();
            it->Join();
        }
        thread_list_.clear();
        pools_count_ = 0;
        return 0;
    }

private:
    int pools_count_{0};
    std::vector<std::shared_ptr<Thread> > thread_list_;
};
}
