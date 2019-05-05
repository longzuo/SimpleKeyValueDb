#ifndef SDB_DB_HPP
#define SDB_DB_HPP
#include <chrono>
#include <ctime>
#include <fstream>
#include <map>
#include <shared_mutex>
#include <unordered_set>
#include "../util/FileUtil.hpp"
#include "../util/StringUtil.hpp"
#include "SDBObject.hpp"
namespace SDB {

enum class IntegerEncoding { INTENC8 = 12, INTENC16, INTENC32, INTENC64 };
const unsigned char EXPIRE = 16;
class Db {
   private:
    std::map<std::string, SDBObject::ObjPointer> db;
    std::unordered_map<std::string, std::time_t> expire;

    int checkOutOfDate(const std::string&);
    void saveObjectType(std::ofstream&, const SDBObject::ObjPointer&);
    void saveStringObject(std::ofstream&, const SDBObject::ObjPointer&);
    void saveHashObject(std::ofstream&, const SDBObject::ObjPointer&);
    void saveListObject(std::ofstream&, const SDBObject::ObjPointer&);
    void saveOsetObject(std::ofstream&, const SDBObject::ObjPointer&);
    void saveSetObject(std::ofstream&, const SDBObject::ObjPointer&);

    void loadStringObject(std::ifstream&, bool, std::string&);
    void loadHashObject(std::ifstream&, bool, std::string&);
    void loadListObject(std::ifstream&, bool, std::string&);
    void loadOsetObject(std::ifstream&, bool, std::string&);
    void loadSetObject(std::ifstream&, bool, std::string&);
    // void loadFrom(const std::string&);

   public:
    void add(const std::string& key, const SDBObject::ObjPointer& objptr);
    void add(std::string&& key, const SDBObject::ObjPointer& objptr);
    void addExpire(const std::string& key, const std::string&);
    void addPrecisionExpire(const std::string& key, const std::string&);
    void delExpire(const std::string& key);
    void del(const std::string& key);
    void save(const std::string&);
    void load(const std::string&);

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

int Db::checkOutOfDate(const std::string& key) {
    using namespace std::chrono;
    auto eit = this->expire.find(key);
    //存在于expire中
    if (eit != this->expire.end()) {
        auto now = system_clock::now();
        //已经过期，返回1
        if (now > system_clock::from_time_t(eit->second)) {
            return 1;
        } else {
            //未过期，返回-1
            return -1;
        }
    }
    //未找到，返回0
    return 0;
}

SDBObject::ObjPointer Db::find(const std::string& key) {
    if (checkOutOfDate(key) == 1) {
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
void Db::saveObjectType(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    saveInteger(ofs, static_cast<unsigned char>(obj->objtype));
}
/*
.sdb文件
|SDB|db_version|key-value|EOF|

key-value
    |expire|time_t|type|key|value|
    |type|key|value|
key
    raw string
value
    string,int

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
    saveInteger(ofs, static_cast<unsigned char>(obj->enctype));
    if (obj->enctype == SdbEncType::SDB_ENC_INT) {
        // long
        saveInteger(ofs, static_cast<unsigned long>(obj->value.istr));
    } else {
        // string
        saveString(ofs, *obj->value.str);
    }
}

void Db::saveHashObject(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    // hash object
    auto umap = obj->value.dict->umap;
    saveInteger(ofs, umap.size());
    for (auto it = umap.begin(); it != umap.end(); ++it) {
        saveString(ofs, it->first);
        saveString(ofs, it->second);
    }
}

void Db::saveListObject(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    // list object
    // auto head=obj->value.list->first;
    saveInteger(ofs, obj->value.list->len());
    auto head = (*(obj->value.list)).first();
    while (head.get()) {
        saveString(ofs, head->data);
        head = head->next;
    }
}

void Db::saveSetObject(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    // set object
    auto uset = obj->value.uset->uset;
    saveInteger(ofs, uset.size());
    for (auto it = uset.begin(); it != uset.end(); ++it) {
        saveString(ofs, *it);
    }
}

void Db::saveOsetObject(std::ofstream& ofs, const SDBObject::ObjPointer& obj) {
    // ordered set object
    auto sklist = obj->value.oset->slist;
    saveInteger(ofs, sklist.len());
    auto ptr = sklist.first();
    while (ptr.get()) {
        saveString(ofs, std::to_string(ptr->score));
        saveString(ofs, ptr->data);
        ptr = ptr->next();
    }
}

void Db::save(const std::string& filename) {
    std::ofstream ofs(filename + ".sdb", std::ofstream::out |
                                             std::ofstream::trunc |
                                             std::ofstream::binary);
    unsigned int version = 1;
    ofs << "SDB";
    saveInteger(ofs, version);
    auto expireCount = expire.size();
    for (auto dbit = db.begin(); dbit != db.end();) {
        //如果还有过期的时间记录，判断当前需要写入的 key-value 是否过期
        if (expireCount > 0) {
            int res = checkOutOfDate(dbit->first);
            if (res == 1) {
                expire.erase(dbit->first);
                // auto newit=db.erase(dbit->first);
                db.erase(dbit++);
                //直接continue，跳过写入
                continue;
            } else if (res == -1) {
                //有过期时间，写入
                saveInteger(ofs, EXPIRE);
                saveInteger(ofs,
                            static_cast<unsigned long>(expire[dbit->first]));
            }
        }
        // write to file
        // save type
        saveObjectType(ofs, dbit->second);
        // save key
        saveString(ofs, dbit->first);
        // std::cout<<"save key:"<<dbit->first<<std::endl;
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
        ++dbit;
    }
    ofs.close();
}
void Db::loadStringObject(std::ifstream& ifs, bool tobesave, std::string& key) {
    //
    char etype = 0;
    loadInteger(ifs, etype);
    SdbEncType enctype = static_cast<SdbEncType>(etype);
    if (enctype == SdbEncType::SDB_ENC_INT) {
        long istr = 0;
        loadInteger(ifs, istr);
        if (tobesave) {
            auto& ptr = this->db[std::move(key)];
            if (!ptr.get()) {
                ptr = SDBObject::CreateStrObject();
            }
            ptr->set(istr);
        }
    } else if (enctype == SdbEncType::SDB_ENC_RAW) {
        std::string str;
        long size = 0;
        loadInteger(ifs, size);
        loadString(ifs, str, size);
        if (tobesave) {
            // this->db[std::move(key)]->set(std::move(str));
            auto& ptr = this->db[std::move(key)];
            if (!ptr.get()) {
                ptr = SDBObject::CreateStrObject();
            }
            ptr->set(std::move(str));
        }
    }
}
void Db::loadListObject(std::ifstream& ifs, bool tobesave, std::string& key) {
    //
    long length = 0;
    loadInteger(ifs, length);
    auto& ptr = this->db[std::move(key)];
    if (tobesave) {
        if (!ptr.get()) {
            ptr = SDBObject::CreateListObject();
        }
    }
    while (length > 0 && ifs.peek() != EOF) {
        long strsize=0;
        std::string str;
        loadInteger(ifs,strsize);
        loadString(ifs,str,strsize);
        if(tobesave){
            ptr->push(std::move(str));
        }
        length--;
    }
}
void Db::loadHashObject(std::ifstream& ifs, bool, std::string&) {}
void Db::loadOsetObject(std::ifstream&, bool, std::string&) {}
void Db::loadSetObject(std::ifstream&, bool, std::string&) {}
void Db::load(const std::string& filename) {
    std::ifstream ifs(filename, std::ifstream::in | std::ifstream::binary);
    if (!ifs.good()) {
        throw SdbException("no such file!");
    }
    std::string sdb;
    loadString(ifs, sdb, 3);
    std::cout << "dbname:" << sdb << std::endl;
    int dbversion = 0;
    loadInteger(ifs, dbversion);
    std::cout << "db version:" << dbversion << std::endl;
    while (ifs.peek() != EOF) {
        char typeOrExpire = 0;
        bool hasExpire = false;
        bool tobesave = true;
        loadInteger(ifs, typeOrExpire);
        long etime = 0;
        if (typeOrExpire == EXPIRE) {
            std::cout << "expire detected" << std::endl;
            loadInteger(ifs, etime);
            if (std::chrono::system_clock::now() <
                std::chrono::system_clock::from_time_t(etime)) {
                hasExpire = true;
            } else {
                tobesave = false;
            }
            loadInteger(ifs, typeOrExpire);
        }
        std::cout << "type:" << static_cast<int>(typeOrExpire) << std::endl;
        std::string key;
        long keysize = 0;
        loadInteger(ifs, keysize);
        std::cout << "keysize:" << keysize << std::endl;
        loadString(ifs, key, keysize);
        std::cout << "key:" << key << std::endl;
        if (hasExpire) {
            this->expire.insert({key, etime});
        }
        switch (static_cast<SdbObjType>(typeOrExpire)) {
            case SdbObjType::SDB_STRING:
                loadStringObject(ifs, tobesave, key);
                break;
            case SdbObjType::SDB_LIST:
                loadListObject(ifs, tobesave, key);
                break;
        }
    }
}

}  // namespace SDB
#endif