#ifndef SDB_EVENTLOOP_HPP
#define SDB_EVENTLOOP_HPP
#include <chrono>
#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include "../util/TimeUtil.hpp"
#include "Epoll.hpp"
namespace SDB {

class EventLoop;

using FileEventProc = std::function<void(EventLoop&, int, void*, int)>;
using TimeEventProc = std::function<int(EventLoop&, long long, void*)>;
using EventFinalizerProc = std::function<void(EventLoop&, void*)>;
using BeforeSleepProc = std::function<void(EventLoop&)>;

class FileEvent {
   public:
    int mask;
    FileEventProc readProc;
    FileEventProc writeProc;
    void* clientData = nullptr;
};

class TimeEvent {
   public:
    long id;
    // long whenSec;
    // long whenMs;
    std::chrono::system_clock::time_point tp;
    TimeEventProc timeProc;
    EventFinalizerProc finalizerProc;
    void* clientData = nullptr;
    // TimeEvent* prev;
    // TimeEvent* next;
    std::shared_ptr<TimeEvent> prev;
    std::shared_ptr<TimeEvent> next;
};

class EventLoop {
   public:
    int maxfd;
    int setsize;
    long timeEventNextId;
    std::chrono::system_clock::time_point lastTime;
    std::map<int, BasicEvent> IOEvents;
    // std::vector<TimeEvent> timeEvents;
    // std::shared_ptr<TimeEvent> timeEventsHead;
    bool stop;
    Epoll eepoll;
    // void* apiData;
    // ApiState apiData;
    BeforeSleepProc beforsleep;
    BeforeSleepProc aftersleep;

    EventLoop(int);
    ~EventLoop() = default;
    int getSetsize() { return this->setsize; }
    bool resizeSetsize(int);
    void stop();
    long createTimeEvent(long, TimeEventProc, EventFinalizerProc);
    bool deleteTimeEvent(long);
    std::shared_ptr<TimeEvent> searchNearestTimer();
    int processTimeEvents();
    int processEvents(int);
    void setBeforeSleepProc(BeforeSleepProc&);
    void setAfterSleepProc(BeforeSleepProc&);
};

EventLoop::EventLoop(int _setsize) {
    this->setsize = _setsize;
    // this->lastTime=time(NULL);
    this->lastTime = TimeUtil::getNowTime();
    this->timeEventNextId = 0;
    this->stop = false;
    this->maxfd = -1;
    this->eepoll.init(this->setsize);
}

void EventLoop::stop() { this->stop = true; }

void EventLoop::setBeforeSleepProc(BeforeSleepProc& beforeProc) {
    this->beforsleep = beforeProc;
}

void EventLoop::setAfterSleepProc(BeforeSleepProc& afterProc) {
    this->aftersleep = afterProc;
}

}  // namespace SDB
#endif