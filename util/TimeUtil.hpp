#ifndef SDB_TIMEUTIL_HPP
#define SDB_TIMEUTIL_HPP
#include <chrono>
using std::chrono::system_clock;
namespace SDB {
class TimeUtil final {
   public:
    TimeUtil() = delete;
    static system_clock::time_point getNowTime();
    static system_clock::time_point addMillisFromNow(long);
    static system_clock::time_point addSecondsFromNow(long);
    static void addSeconds(system_clock::time_point&, long);
    static void addMillis(system_clock::time_point&, long);
    static long getDiffMillis(system_clock::time_point,
                              system_clock::time_point);
};

system_clock::time_point TimeUtil::getNowTime() {
    return std::chrono::system_clock::now();
}

system_clock::time_point TimeUtil::addMillisFromNow(long millis) {
    auto tp = std::chrono::system_clock::now();
    auto dura = std::chrono::duration<long, std::milli>(millis);
    tp += dura;
    return tp;
}

system_clock::time_point TimeUtil::addSecondsFromNow(long seconds) {
    auto tp = std::chrono::system_clock::now();
    auto dura = std::chrono::duration<long>(seconds);
    tp += dura;
    return tp;
}

void TimeUtil::addSeconds(system_clock::time_point& tp, long seconds) {
    auto dura = std::chrono::duration<long>(seconds);
    tp += dura;
}

void TimeUtil::addMillis(system_clock::time_point& tp, long millis) {
    auto dura = std::chrono::duration<long, std::milli>(millis);
    tp += dura;
}

long TimeUtil::getDiffMillis(system_clock::time_point t0,
                             system_clock::time_point t1) {
    auto nanos = t1 - t0;
    return (nanos.count() / 1000000);
}

}  // namespace SDB
#endif