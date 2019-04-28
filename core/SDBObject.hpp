#ifndef SDB_SDBOBJECT_HPP
#define SDB_SDBOBJECT_HPP
#include "Dict.hpp"
#include "IntSet.hpp"
#include "List.hpp"
#include "SkipList.hpp"

namespace SDB {
enum class SdbObjType {
    //字符串
    SDB_STRING,
    //列表
    SDB_LIST,
    //哈希
    SDB_HASH,
    //集合
    SDB_SET,
    //有序集合
    SDB_OSET,
    //空object
    SDB_NULL
};
enum class SdbEncType {
    SDB_ENC_INT,
    SDB_ENC_HT,
    SDB_ENC_RAW,
    SDB_ENC_LINKEDLIST,
    SDB_ENC_INTSET,
    SDB_ENC_SKIPLIST
};
class SDBObject {
   private:
    SdbObjType objtype;
    SdbEncType enctype;
    unsigned int lru;
    union {
        long istr;
        std::string* str;
        List<SDBObject>* list;
        SkipList<SDBObject>* sklist;
        Dict<SDBObject>* dict;
        IntSet* iset;
    } value;

   public:
    SDBObject();
    ~SDBObject();
    typedef std::shared_ptr<SDBObject> ObjPointer;
    static std::shared_ptr<SDBObject> CreateStrObject();
    // static std::shared_ptr<SDBObject> CreateSetObject();
    static std::shared_ptr<SDBObject> CreateListObject();
    static std::shared_ptr<SDBObject> CreateHashObject();
    static std::shared_ptr<SDBObject> CreateOSetObject();

    //for object
    std::string getObjType();
    std::string getEncType();

    // for String
    void set(const int64_t&);
    void set(std::string&&);
    void append(const std::string&);
    ssize_t getStrLen();

    // SDBObject& operator=(const SDBObject&);

    void print(std::ostream& out = std::cout);
};

SDBObject::SDBObject() { this->objtype = SdbObjType::SDB_NULL; }
SDBObject::~SDBObject() {
    switch (this->objtype) {
        case SdbObjType::SDB_STRING:
            if (this->enctype != SdbEncType::SDB_ENC_INT) {
                delete value.str;
            }
            break;
        case SdbObjType::SDB_HASH:
            delete value.dict;
            break;
        case SdbObjType::SDB_LIST:
            delete value.list;
            break;
        case SdbObjType::SDB_OSET:
            delete value.sklist;
            break;
        default:
            break;
    }
}
std::shared_ptr<SDBObject> SDBObject::CreateStrObject() {
    return std::make_shared<SDBObject>();
}

std::shared_ptr<SDBObject> SDBObject::CreateListObject() {
    std::shared_ptr<SDBObject> temp = std::make_shared<SDBObject>();
    temp->enctype = SdbEncType::SDB_ENC_LINKEDLIST;
    temp->objtype = SdbObjType::SDB_LIST;
    return temp;
}

std::shared_ptr<SDBObject> SDBObject::CreateHashObject() {
    std::shared_ptr<SDBObject> temp = std::make_shared<SDBObject>();
    temp->enctype = SdbEncType::SDB_ENC_HT;
    temp->objtype = SdbObjType::SDB_HASH;
    return temp;
}

std::shared_ptr<SDBObject> SDBObject::CreateOSetObject() {
    std::shared_ptr<SDBObject> temp = std::make_shared<SDBObject>();
    temp->enctype = SdbEncType::SDB_ENC_SKIPLIST;
    temp->objtype = SdbObjType::SDB_OSET;
    return temp;
}

std::string SDBObject::getObjType(){
    std::string res;
    switch(this->objtype){
        case SdbObjType::SDB_STRING:
            res="string";
            break;
        case SdbObjType::SDB_HASH:
            res="hash";
            break;
        case SdbObjType::SDB_SET:
            res="set";
            break;
        case SdbObjType::SDB_NULL:
            res="null";
            break;
        case SdbObjType::SDB_OSET:
            res="ordered set";
            break;
        case SdbObjType::SDB_LIST:
            res="list";
            break;
    }
    return res;
}

std::string SDBObject::getEncType(){
    std::string res;
    switch(this->enctype){
        case SdbEncType::SDB_ENC_INT:
            res="int";
            break;
        case SdbEncType::SDB_ENC_RAW:
            res="raw";
            break;
        case SdbEncType::SDB_ENC_INTSET:
            res="intset";
            break;
        case SdbEncType::SDB_ENC_LINKEDLIST:
            res="linkedlist";
            break;
        case SdbEncType::SDB_ENC_SKIPLIST:
            res="skiplist";
            break;
        case SdbEncType::SDB_ENC_HT:
            res="hashtable";
            break;
    }
    return res;
}

void SDBObject::set(const int64_t& data) {
    if (this->objtype == SdbObjType::SDB_NULL) {
        this->objtype = SdbObjType::SDB_STRING;
        this->enctype = SdbEncType::SDB_ENC_INT;
    }
    if (this->objtype == SdbObjType::SDB_STRING) {
        if (this->enctype == SdbEncType::SDB_ENC_RAW) {
            delete this->value.str;
        }
        this->value.istr = data;
        this->enctype = SdbEncType::SDB_ENC_INT;
    } else {
        // error
    }
}

void SDBObject::set(std::string&& s) {
    if (this->objtype == SdbObjType::SDB_NULL) {
        this->objtype = SdbObjType::SDB_STRING;
        this->enctype = SdbEncType::SDB_ENC_RAW;
        this->value.str = nullptr;
    }
    if (this->objtype == SdbObjType::SDB_STRING) {
        if (this->enctype != SdbEncType::SDB_ENC_INT) {
            if (this->value.str) delete this->value.str;
        }
        this->value.str = new std::string(s);
        this->enctype = SdbEncType::SDB_ENC_RAW;
    } else {
        // error
    }
}

void SDBObject::append(const std::string& data) {
    if (this->objtype == SdbObjType::SDB_STRING ||
        this->objtype == SdbObjType::SDB_NULL) {
        if(this->objtype==SdbObjType::SDB_NULL){
            this->objtype= SdbObjType::SDB_STRING;
            this->value.str = new std::string;
        }else if (this->enctype == SdbEncType::SDB_ENC_INT) {
            long temp = this->value.istr;
            this->value.str = new std::string;
            this->value.str->append(std::to_string(temp));
        } else if (this->value.str == nullptr) {
            this->value.str = new std::string;
        }
        this->enctype = SdbEncType::SDB_ENC_RAW;
        this->value.str->append(data);
    } else {
        // error
    }
}

ssize_t SDBObject::getStrLen(){
    if(this->objtype==SdbObjType::SDB_STRING){
        if(this->enctype==SdbEncType::SDB_ENC_RAW){
            return this->value.str->size();
        }else{
            int64_t temp=this->value.istr;
            int64_t length=0;
            if(temp<0){
                temp=0-temp;
                ++length;
            }
            while(temp){
                ++length;
                temp/=10;
            }
            return length;
        }
    }else{
        //error
    }
}

void SDBObject::print(std::ostream& out) {
    if (this->objtype == SdbObjType::SDB_STRING) {
        if (this->enctype == SdbEncType::SDB_ENC_INT) {
            out << this->value.istr << std::endl;
        } else {
            out << *(this->value.str) << std::endl;
        }
    }else{
        out<<"unknown type"<<std::endl;
    }
}

}  // namespace SDB

#endif