#ifndef SDB_DB_HPP
#define SDB_DB_HPP
#include <chrono>
#include <ctime>
#include <fstream>
#include <map>
#include <shared_mutex>
#include <unordered_set>
#include "../util/StringUtil.hpp"
#include "SDBObject.hpp"
namespace SDB {

enum class IntegerEncoding { INTENC8, INTENC16, INTENC32, INTENC64 };

class Db {
   private:
    std::map<std::string, SDBObject::ObjPointer> db;
    std::unordered_map<std::string, std::time_t> expire;

    bool checkOutOfDate(const std::string&);
    void saveInteger(std::ofstream&, const unsigned long);
    void saveInteger(std::ofstream&, const unsigned int);
    void saveInteger(std::ofstream&, const unsigned char);
    void saveString(std::ofstream&, const std::string&);
    void saveObjectType(std::ofstream&, const SDBObject::ObjPointer&);
    void saveStringObject(std::ofstream&, const SDBObject::ObjPointer&);
    void saveHashObject(std::ofstream&, const SDBObject::ObjPointer&);
    void saveListObject(std::ofstream&, const SDBObject::ObjPointer&);
    void saveOsetObject(std::ofstream&, const SDBObject::ObjPointer&);
    void saveSetObject(std::ofstream&, const SDBObject::ObjPointer&);

   public:
    void add(const std::string& key, const SDBObject::ObjPointer& objptr);
    void add(std::string&& key, const SDBObject::ObjPointer& objptr);
    void addExpire(const std::string& key, const std::string&);
    void addPrecisionExpire(const std::string& key, const std::string&);
    void delExpire(const std::string& key);
    void del(const std::string& key);
    void save(const std::string&);
    void load(const std::string& filename);

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
    system_clock::duration ttl = duration<long>(sec);
    auto deadline = system_clock::now() + ttl;
    auto timet = system_clock::to_time_t(deadline);
    this->expire.insert({key, timet});
}

void Db::addPrecisionExpire(const std::string& key, const std::string& millis) {
    using namespace std::chrono;
    long milliseconds = StringUtil::toDouble(millis);
    system_clock::duration ttl = duration<long, std::milli>(milliseconds);
    auto deadline = system_clock::now() + ttl;
    auto timet = system_clock::to_time_t(deadline);
    this->expire.insert({key, timet});
}

void Db::del(const std::string& key) { db.erase(key); }

bool Db::checkOutOfDate(const std::string& key) {
    using namespace std::chrono;
    auto eit = this->expire.find(key);
    if (eit != this->expire.end()) {
        auto now = system_clock::now();
        if (now > system_clock::from_time_t(eit->second)) {
            return true;
        }
    }
    return false;
}

SDBObject::ObjPointer Db::find(const std::string& key) {
    if (checkOutOfDate(key)) {
        this->expire.erase(key);
        this->db.erase(key);
        return nullptr;
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
void Db::saveString(std::ofstream& ofs, const std::string& key) {
    size_t len = key.size();
    //ofs.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
    saveInteger(ofs,len);
    ofs << key;
}
void Db::saveInteger(std::ofstream& ofs, const unsigned long data) {
    ofs.write(reinterpret_cast<const char*>(&data), sizeof(long));
}
void Db::saveInteger(std::ofstream& ofs, const unsigned int data) {
    ofs.write(reinterpret_cast<const char*>(&data), sizeof(int));
}
void Db::saveInteger(std::ofstream& ofs, const unsigned char data) {
    ofs.write(reinterpret_cast<const char*>(&data), sizeof(char));
}

void Db::saveObjectType(std::ofstream& ofs, const SDBObject::ObjPointer& obj){
    saveInteger(ofs,static_cast<unsigned char>(obj->objtype));
}
/*
.sdb文件
|SDB|db_version|key-value|EOF|

string
|size|str value|

string object
|enc|value|
enc:raw,int
value:
    raw:string
    int:long

*/
void Db::saveStringObject(std::ofstream& ofs,
                          const SDBObject::ObjPointer& obj) {
    saveInteger(ofs,static_cast<unsigned char>(obj->enctype));
    if (obj->enctype == SdbEncType::SDB_ENC_INT) {
        // long
        saveInteger(ofs,static_cast<unsigned long>(obj->value.istr));
    } else {
        // string
        saveString(ofs,*obj->value.str);
    }
}

void Db::saveHashObject(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    //hash object
    auto umap=obj->value.dict->umap;
    saveInteger(ofs,umap.size());
    for(auto it=umap.begin();it!=umap.end();++it){
        saveString(ofs,it->first);
        saveString(ofs,it->second);
    }
}

void Db::saveListObject(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    //list object
    //auto head=obj->value.list->first;
    saveInteger(ofs,obj->value.list->len());
    auto head=(*(obj->value.list)).first();
    while(head.get()){
        saveString(ofs,head->data);
        head=head->next;
    }    
}

void Db::saveSetObject(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    //set object
    auto uset=obj->value.uset->uset;
    saveInteger(ofs,uset.size());
    for(auto it=uset.begin();it!=uset.end();++it){
        saveString(ofs,*it);
    }
}

void Db::saveOsetObject(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    //ordered set object
    auto sklist=obj->value.oset->slist;
    saveInteger(ofs,sklist.len());
    auto ptr=sklist.first();
    while(ptr.get()){
        saveString(ofs,std::to_string(ptr->score));
        saveString(ofs,ptr->data);
        ptr=ptr->next();
    }
}

void Db::save(const std::string& filename) {
    std::ofstream ofs(filename + ".sdb", std::ofstream::out |
                                             std::ofstream::trunc |
                                             std::ofstream::binary);
    int version = 1;
    ofs << "SDB" << version;
    auto expireCount = expire.size();
    for (auto dbit = db.begin(); dbit != db.end(); ++dbit) {
        //如果还有过期的时间记录，判断当前需要写入的 key-value 是否过期
        if (expireCount > 0) {
            if (checkOutOfDate(dbit->first)) {
                expire.erase(dbit->first);
                // auto newit=db.erase(dbit->first);
                db.erase(dbit++);
                //直接continue，跳过写入
                continue;
            }
        }
        // write to file
        //save type
        saveObjectType(ofs, dbit->second);
        //save key
        saveString(ofs, dbit->first);
        switch (dbit->second->getEnumObjType()) {
            case SdbObjType::SDB_STRING:
                saveStringObject(ofs, dbit->second);
                break;
            case SdbObjType::SDB_HASH:
                saveHashObject(ofs, dbit->second);
                break;
            case SdbObjType::SDB_LIST:
                saveListObject(ofs, dbit->second);
                break;
            case SdbObjType::SDB_SET:
                saveSetObject(ofs, dbit->second);
                break;
            case SdbObjType::SDB_OSET:
                saveOsetObject(ofs, dbit->second);
                break;
            default:
                break;
        }
    }
    ofs.close();
}

}  // namespace SDB
#endif