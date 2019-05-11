#ifndef SDB_EPOLL_HPP
#define SDB_EPOLL_HPP
#include <errno.h>
#include <vector>
#include "../core/Exception.hpp"
#include "IOEvent.hpp"
#include "NetLib.hpp"
#include "unix_header.h"
namespace Net {

class EpollEvents {
   public:
    struct epoll_event* events;
    int length;
    int capcity;
    int maxEvent;
    int size() { return length; }
    int maxEvents() { return maxEvent; }
    void reserve(int);
    void resize(int);
    void addEvent();
    void clear() { length = 0; }
    EpollEvents() : length(0), capcity(0), maxEvent(0), events(nullptr) {}
    EpollEvents(const EpollEvents&);
    EpollEvents(EpollEvents&&);
    EpollEvents& operator=(const EpollEvents&);
    EpollEvents& operator=(EpollEvents&&);
    epoll_event& operator[](int);
};

void EpollEvents::reserve(int nsize) {
    if (nsize <= length) {
        return;
    }
    epoll_event* nptr = new epoll_event[nsize];
    for (int i = 0; i < length; i++) {
        nptr[i] = events[i];
    }
    if (events != nullptr) {
        delete[] events;
    }
    events = nptr;
    capcity = nsize;
}

void EpollEvents::resize(int nsize) {
    if (capcity < nsize) {
        reserve(nsize);
    }
    length = nsize;
}

void EpollEvents::addEvent() {
    if (maxEvent + 1 > capcity) {
        reserve((maxEvent + 1) * 2);
    }
    maxEvent += 1;
}

EpollEvents::EpollEvents(const EpollEvents& oth) {
    if (maxEvent < oth.maxEvent) {
        reserve(oth.maxEvent);
    }
    for (int i = 0; i < oth.length; i++) {
        events[i] = oth.events[i];
    }
    length = oth.length;
    maxEvent = oth.maxEvent;
}

EpollEvents::EpollEvents(EpollEvents&& oth) {
    length = oth.length;
    capcity = oth.capcity;
    events = oth.events;
    maxEvent = oth.maxEvent;
    oth.length = 0;
    oth.capcity = 0;
    oth.events = nullptr;
    oth.maxEvent = 0;
}

EpollEvents& EpollEvents::operator=(const EpollEvents& oth) {
    if (maxEvent < oth.maxEvent) {
        reserve(oth.maxEvent);
    }
    for (int i = 0; i < oth.length; i++) {
        events[i] = oth.events[i];
    }
    length = oth.length;
    maxEvent = oth.maxEvent;
    return *this;
}

EpollEvents& EpollEvents::operator=(EpollEvents&& oth) {
    length = oth.length;
    capcity = oth.capcity;
    events = oth.events;
    maxEvent = oth.maxEvent;
    oth.length = 0;
    oth.capcity = 0;
    oth.events = nullptr;
    oth.maxEvent = 0;
    return *this;
}

epoll_event& EpollEvents::operator[](int pos) {
    if (pos >= length) {
        throw NetException("out of index of epoll events");
    }
    return events[pos];
}

class Epoll final {
   private:
    int epfd;

   public:
    Epoll();
    ~Epoll();
    bool addEvent(IOEvent&);
    bool delEvent(IOEvent&);
    bool modifyEvent(IOEvent&);
    int waitEvents(EpollEvents&, int);
};

Epoll::Epoll() {
    this->epfd = epoll_create(1024);
    if (this->epfd == -1) {
        throw NetException("can not create epoll!");
    }
}

Epoll::~Epoll() {
    if (epfd != -1) {
        ::close(epfd);
    }
}

bool Epoll::addEvent(IOEvent& e) {
    epoll_event ee = {0, 0};
    if (e.getmask() & EventStatus::EVENT_READABLE) {
        ee.events |= EPOLLIN;
    }
    if (e.getmask() & EventStatus::EVENT_WRITEABLE) {
        ee.events |= EPOLLOUT;
    }
    // int option;
    ee.data.fd = e.getfd();
    if (epoll_ctl(this->epfd, EPOLL_CTL_ADD, e.getfd(), &ee) == -1) {
        return false;
    }
    return true;
}

bool Epoll::delEvent(IOEvent& e) {
    return (epoll_ctl(this->epfd, EPOLL_CTL_DEL, e.getfd(), nullptr) != -1);
}

bool Epoll::modifyEvent(IOEvent& e) {
    epoll_event ee = {0, 0};
    ee.events = 0;
    if (e.getmask() & EventStatus::EVENT_READABLE) {
        ee.events |= EPOLLIN;
    }
    if (e.getmask() & EventStatus::EVENT_WRITEABLE) {
        ee.events |= EPOLLOUT;
    }
    ee.data.fd = e.getfd();
    if (e.getmask() != EventStatus::EVENT_NONE) {
        return (epoll_ctl(this->epfd, EPOLL_CTL_MOD, e.getfd(), &ee) != -1);
    } else {
        return (epoll_ctl(this->epfd, EPOLL_CTL_DEL, e.getfd(), &ee) != -1);
    }
}

int Epoll::waitEvents(EpollEvents& fired, int timeout) {
    int numevents = 0;
    numevents =
        epoll_wait(this->epfd, fired.events, fired.maxEvents(), timeout);
    if (numevents < 0) {
        throw NetException("epoll error,errno is:" + std::to_string(errno));
    }
    // fired.resize()
    fired.resize(numevents);
    return numevents;
}

}  // namespace Net
#endif