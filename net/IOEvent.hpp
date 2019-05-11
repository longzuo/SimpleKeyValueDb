#ifndef NET_IOEVENT_HPP
#define NET_IOEVENT_HPP
#include <functional>
#include <memory>
#include "Event.hpp"
#include "unix_header.h"
namespace Net {

using IODefaultFunc = std::function<void()>;

class IOEvent {
   private:
    int fd;
    int mask;
    IODefaultFunc readFunc;
    IODefaultFunc wirteFunc;
    IODefaultFunc closeFunc;

   public:
    IOEvent(int _fd) : fd(_fd), mask(0) {}
    void setReadFunc(const IODefaultFunc&);
    void setWriteFunc(const IODefaultFunc&);
    void setCloseFunc(const IODefaultFunc&);
    void handleEvent(uint32_t);
    int getfd() { return fd; }
    int getmask() { return mask; }
    void setReadable() { mask |= EventStatus::EVENT_READABLE; }
    void setWriteable() { mask |= EventStatus::EVENT_WRITEABLE; }
    void unsetReadable() { mask &= (~EventStatus::EVENT_READABLE); }
    void unsetWriteable() { mask &= (~EventStatus::EVENT_WRITEABLE); }
    void setMask(int mk) { mask = mk; }
};

void IOEvent::setReadFunc(const IODefaultFunc& rf) {
    //
    readFunc = rf;
}

void IOEvent::setWriteFunc(const IODefaultFunc& wf) {
    //
    wirteFunc = wf;
}

void IOEvent::setCloseFunc(const IODefaultFunc& cf) {
    // close
    closeFunc = cf;
}

void IOEvent::handleEvent(uint32_t events) {
    if ((events & EPOLLHUP) && !(events & EPOLLIN)) {
        if (closeFunc) closeFunc();
    }

    if (events & EPOLLERR || events & EPOLLRDHUP) {
        if (closeFunc) closeFunc();
    }
    if (events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (readFunc) readFunc();
    }
    if (events & EPOLLOUT) {
        if (wirteFunc) wirteFunc();
    }
}

using IOEventPtr = std::shared_ptr<IOEvent>;

}  // namespace Net

#endif