#ifndef SDB_DB_HPP
#define SDB_DB_HPP
#include <map>
#include "SDBObject.hpp"
namespace SDB {
class Db {
   private:
    std::map<std::string, SDBObject::ObjPointer> db;

   public:
    void add(const std::string& key, const SDBObject::ObjPointer& objptr);
    void add(std::string&& key, const SDBObject::ObjPointer& objptr);
    void del(const std::string& key);
    SDBObject::ObjPointer find(const std::string&);
    SDBObject::ObjPointer& operator[](const std::string&);
    SDBObject::ObjPointer& operator[](std::string&&);
};

void Db::add(const std::string& key, const SDBObject::ObjPointer& objptr) {
    db.insert({key, objptr});
}

void Db::add(std::string&& key, const SDBObject::ObjPointer& objptr) {
    db[std::move(key)]=objptr;
}

void Db::del(const std::string& key) { db.erase(key); }

SDBObject::ObjPointer Db::find(const std::string& key) {
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