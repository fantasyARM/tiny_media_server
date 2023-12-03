#pragma once
#include <memory>
#include <mutex>
#include "event.h"
#include "noncopyable.hpp"
#include "thread_pool.hpp"
namespace tiny
{
class EventEngine: Noncopyable
{
public:
    static constexpr int MAX_EPOLL_FDS = 1024000;
    static constexpr int MAX_EPOLL_EVENTS = 100;
    explicit EventEngine(const int pools = 0) {
        InitEngine(pools);
    }
    virtual ~EventEngine() {
        Release();
    }

    int AddEvent(const int t, Processor *p);

    int DelEvent(const std::shared_ptr<Event>& e) {
        std::lock_guard<std::recursive_mutex> lg(lk_);
	trigle_list_.erase(e->GetFd());
	delete e->Handle();
        return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, e->GetFd(), NULL);
    }

    int RunOnce();

    int RunLoop();

    void SetStop();

    int Release() {
        std::lock_guard<std::recursive_mutex> lg(lk_);
        CLOSE(epoll_fd_);
        trigle_list_.clear();
        if (pools_)
            pools_->StopPools();
    }
    int WaitEpoll();

    int InsertEpoll(const std::shared_ptr<Event>& e);

    int ModEvent(const std::shared_ptr<Event>& e);

    int HandleEvent(const std::shared_ptr<Event>& e);

private:
    int InitEngine(const int pools = 0);
    Processor::NotifyCall engine_notify_;
    Processor::NotifyCall engine_add_;
    std::atomic<bool> running_{true};
    std::unique_ptr<ThreadPools> pools_;
    std::recursive_mutex lk_;
    std::map<int, std::shared_ptr<Event> > trigle_list_;
    int epoll_fd_{-1}; 
};
}
