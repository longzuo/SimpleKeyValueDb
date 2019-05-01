#ifndef SDB_DB_HPP
#define SDB_DB_HPP
#include <chrono>
#include <map>
#include <unordered_set>
#include "../executor/StringUtil.hpp"
#include "SDBObject.hpp"
namespace SDB {
class Db {
   private:
    std::map<std::string, SDBObject::ObjPointer> db;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point>
        expire;

   public:
    void add(const std::string& key, const SDBObject::ObjPointer& objptr);
    void add(std::string&& key, const SDBObject::ObjPointer& objptr);
    void addExpire(const std::string& key, const std::string&);
    void addPrecisionExpire(const std::string& key, const std::string&);
    void delExpire(const std::string& key);
    void del(const std::string& key);
    SDBObject::ObjPointer find(const std::string&);
    SDBObject::ObjPointer& operator[](const std::string&);
    SDBObject::ObjPointer& operator[](std::string&&);
};

void Db::add(const std::string& key, const SDBObject::ObjPointer& objptr) {
    db.insert({key, objptr});
}

void Db::add(std::string&& key, const SDBObject::ObjPointer& objptr) {
    db[std::move(key)] = objptr;
}

void Db::addExpire(const std::string& key, const std::string& seconds) {
    using namespace std::chrono;
    long sec = StringUtil::toDouble(seconds);
    steady_clock::duration ttl = duration<long>(sec);
    auto deadline = steady_clock::now() + ttl;
    this->expire.insert({key, deadline});
}

void Db::addPrecisionExpire(const std::string& key, const std::string& millis) {
    using namespace std::chrono;
    long milliseconds = StringUtil::toDouble(millis);
    steady_clock::duration ttl = duration<long, std::milli>(milliseconds);
    auto deadline = steady_clock::now() + ttl;
    this->expire.insert({key, deadline});
}

void Db::del(const std::string& key) { db.erase(key); }

SDBObject::ObjPointer Db::find(const std::string& key) {
    using namespace std::chrono;
    auto eit = this->expire.find(key);
    if (eit != this->expire.end()) {
        auto now = steady_clock::now();
        if (now > eit->second) {
            this->expire.erase(key);
            this->db.erase(key);
            return nullptr;
        }
    }
    auto it = db.find(key);
    if (it != db.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

SDBObject::ObjPointer& Db::operator[](const std::string& key) {
    return db[key];
}

SDBObject::ObjPointer& Db::operator[](std::string&& key) {
    return db[std::move(key)];
}
}  // namespace SDB
#endif