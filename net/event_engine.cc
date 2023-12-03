#include "event_engine.h"
using namespace tiny;

int EventEngine::InitEngine(const int pools)
{
    int ret = 0;
    int fd = -1;
    defer [&] {
        if (ret != 0) {
	    CLOSE(fd);
	} 
    };
    fd = epoll_create(MAX_EPOLL_FDS);
    if (fd < 0) {
        RET(TINY_ERROR_EPOLLCREATE);
    }
    if (pools > 0) {
       pools_ = make_unique<ThreadPools>(pools); 
       pools_->InitPools();
    }
    epoll_fd_ = fd;
    engine_notify_ = [&](int ret, const std::shared_ptr<Event> &ee) {
            if (ret >= 0) {
                this->ModEvent(ee);
            }
            else //process not ok
                this->DelEvent(ee);
        };
    engine_add_ = [&](int ret, const std::shared_ptr<Event> &ee) {
            this->InsertEpoll(ee);
	};
    RET(TINY_SUCCESS);
}

int EventEngine::AddEvent(const int t, Processor *p)
{
    std::lock_guard<std::recursive_mutex> lg(lk_); 
    auto it = trigle_list_.find(p->GetFd());
    if (it == trigle_list_.end()) {
        InsertEpoll(std::make_shared<Event>(t, p));
    } else { //event tepy has change
        ModEvent(std::make_shared<Event>(t, p));
    }
    return 0;
}

int EventEngine::ModEvent(const std::shared_ptr<Event> &e)
{
    struct epoll_event set_events;
    memset(&set_events, 0x0, sizeof(set_events));

    if (e->GetType() & EventType_Stop) {
        return DelEvent(e);
    }

    if (e->GetType() & EventType_Read) {
        set_events.events = EPOLLIN|EPOLLET|EPOLLERR|EPOLLHUP;
    } 
    
    if (e->GetType() & EventType_Write) {
        set_events.events |= EPOLLOUT;
    }

    if (e->GetType() & EventType_Timeout) {
        
    }
    set_events.data.ptr = e->Handle();

    std::lock_guard<std::recursive_mutex> lg(lk_);
    return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, e->GetFd(), &set_events);
}

int EventEngine::InsertEpoll(const std::shared_ptr<Event> &e)
{
    struct epoll_event set_events;
    memset(&set_events, 0x0, sizeof(set_events));
    if (e->GetType() & EventType_Stop) {
        //ignore this
    }

    if (e->GetType() & EventType_Read) {
        set_events.events = EPOLLIN|EPOLLET|EPOLLERR|EPOLLHUP;
    }

    if (e->GetType() & EventType_Write) {
        set_events.events |= EPOLLOUT;
    }
    
    if (e->GetType() & EventType_Timeout) {
        
    }

    set_events.data.ptr = e->Handle(); //set event handler

    std::lock_guard<std::recursive_mutex> lg(lk_);
    trigle_list_.insert(std::pair<int, std::shared_ptr<Event>>(e->GetFd(), e));
    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, e->GetFd(), &set_events);
}

int EventEngine::RunOnce()
{
    WaitEpoll();
}

int EventEngine::RunLoop()
{
    while (running_.load()) {
        WaitEpoll();
        sleep(0);
    }
    return 0;
}

void EventEngine::SetStop()
{
    running_ = false;
}

int EventEngine::HandleEvent(const std::shared_ptr<Event>& e)
{
    if (pools_) {
        pools_->HandleEvent(e);
    } else {
        int ret = 0;
        if (e->GetType() & EPOLLIN) {
            if ((ret = e->Handle()->HandleRead()) != 0) {
	        DelEvent(e);
	        RET(-1);
	    } else {
	        ModEvent(e);
	    }
	}

	if (e->GetType() & EPOLLOUT) {
	    if ((ret = e->Handle()->HandleWrite(e->GetPkt())) != 0) {
	        DelEvent(e);
                RET(-2);
	    } else {
	        ModEvent(e);
	    }
	}

	if (e->GetType() & EPOLLERR) {
	    DelEvent(e); 
	}
    }
    return 0;
}

int EventEngine::WaitEpoll()
{
    struct epoll_event events[MAX_EPOLL_EVENTS];
    memset(events, 0x0, sizeof(events));
    std::lock_guard<std::recursive_mutex> lg(lk_);
    int ret = epoll_wait(epoll_fd_, events, MAX_EPOLL_EVENTS, -1);
    if (ret <= 0) {
        RET(0);
    }

    for (int i = 0; i < ret; i++) {
        int event_type = 0;
        if (events[i].events & EPOLLIN) {
	    event_type |= EventType_Read;
	} 

	if (events[i].events & EPOLLOUT) {
	    event_type |= EventType_Write;
	}

	if (events[i].events & EPOLLERR) { //if any error happend ,stop this 
            event_type |= EventType_Stop;
        }
        // route event to thread pool
    	std::shared_ptr<Event> e = std::make_shared<Event>(event_type, (Processor *)events[i].data.ptr);
        e->Handle()->SetNotify(std::forward<Processor::NotifyCall>(engine_notify_));
        e->Handle()->SetInsert(std::forward<Processor::NotifyCall>(engine_add_));
	HandleEvent(e);
    }
    RET(0);
}

