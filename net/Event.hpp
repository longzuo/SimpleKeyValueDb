#ifndef SDB_EVENT_HPP
#define SDB_EVENT_HPP
#include <chrono>
namespace Net {

class EventStatus final {
   public:
    static const int EVENT_NONE = 0;
    static const int EVENT_READABLE = 1;
    static const int EVENT_WRITEABLE = 2;
    static const int EVENT_BARRIER = 4;
    EventStatus() = delete;
};

class EventType final {
   public:
    static const int FILE_EVENT = 1;
    static const int TIME_EVENT = 2;
    static const int ALL_EVENTS = FILE_EVENT | TIME_EVENT;
    static const int EVENT_DONT_WAIT = 4;
    static const int EVENT_CALL_AFTER_SLEEP = 8;
    EventType() = delete;
};

class TimeEventIdStatus final {
   public:
    static const long TIMEEVENT_DELETED_ID = -1;
    TimeEventIdStatus() = delete;
};

}  // namespace Net
#endif