#ifndef SDB_SDBOBJECT_HPP
#define SDB_SDBOBJECT_HPP
#include <atomic>
#include "Dict.hpp"
#include "Exception.hpp"
#include "IntSet.hpp"
#include "List.hpp"
#include "OrderedSet.hpp"
#include "Set.hpp"
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
class SDBObjectHash;
class SDBObjectCmp;
class SDBObject {
    friend class SDBObjectHash;
    friend class SDBObjectCmp;

   private:
    SdbObjType objtype;
    SdbEncType enctype;
    unsigned int lru;
    std::atomic<int>* count;
    union {
        long istr;
        std::string* str;
        List<std::string>* list;
        Dict<std::string>* dict;
        OrderedSet<std::string>* oset;
        Set<std::string>* uset;
    } value;
    void free();

   public:
    SDBObject();
    SDBObject(const SDBObject&);
    SDBObject& operator=(const SDBObject&);
    ~SDBObject();
    typedef std::shared_ptr<SDBObject> ObjPointer;
    static std::shared_ptr<SDBObject> CreateStrObject();
    static std::shared_ptr<SDBObject> CreateSetObject();
    static std::shared_ptr<SDBObject> CreateListObject();
    static std::shared_ptr<SDBObject> CreateHashObject();
    static std::shared_ptr<SDBObject> CreateOSetObject();

    // for object
    std::string getObjType();
    std::string getEncType();

    // for String
    void set(std::string&&);
    void append(const std::string&);
    ssize_t getStrLen();

    // for list
    void push(std::string&&);
    void pop(std::ostream& out);
    ssize_t llen();

    // for hash
    void hadd(std::string&&, std::string&&);
    void hdel(const std::string&);

    // for ordered set
    void oadd(const double& socre, std::string&&);
    void odel(const double&);

    // for unordered set
    void sadd(std::string&&);
    void sdel(const std::string&);

    // SDBObject& operator=(const SDBObject&);

    void print(std::ostream& out = std::cout) const;
};
/*
struct SDBObjectHash {
    ssize_t operator()(const SDBObject& obj) const noexcept {
        if (obj.objtype == SdbObjType::SDB_STRING) {
            // if()
            return std::hash<std::string>()(*obj.value.str);
        } else {
            //只在无序集合中使用string，这里不应当进入
            return obj.value.istr;
        }
    }
};
struct SDBObjectCmp {
    bool operator()(const SDBObject& l, const SDBObject& r) const noexcept {
        if (l.objtype == SdbObjType::SDB_STRING &&
            r.objtype == SdbObjType::SDB_STRING) {
            if (l.enctype == SdbEncType::SDB_ENC_RAW &&
                r.enctype == SdbEncType::SDB_ENC_RAW) {
                if (l.value.str && r.value.str) {
                    return (*l.value.str) == *(r.value.str);
                }
            } else if (l.enctype == SdbEncType::SDB_ENC_INT &&
                       r.enctype == SdbEncType::SDB_ENC_INT) {
                if (l.value.istr == r.value.istr) {
                    return true;
                }
            }
        }
        return false;
    }
};
*/
SDBObject::SDBObject() {
    this->objtype = SdbObjType::SDB_NULL;
    this->enctype = SdbEncType::SDB_ENC_INT;
    this->count = new std::atomic<int>(1);
}
SDBObject::SDBObject(const SDBObject& oth) {
    this->objtype = oth.objtype;
    this->enctype = oth.enctype;
    this->count = oth.count;
    ++(*oth.count);
    if (this->enctype == SdbEncType::SDB_ENC_INT) {
        this->value.istr = oth.value.istr;
        return;
    }
    switch (this->objtype) {
        case SdbObjType::SDB_STRING:
            this->value.str = oth.value.str;
            break;
        case SdbObjType::SDB_HASH:
            this->value.dict = oth.value.dict;
            break;
        case SdbObjType::SDB_LIST:
            this->value.list = oth.value.list;
            break;
        case SdbObjType::SDB_OSET:
            this->value.oset = oth.value.oset;
            break;
        case SdbObjType::SDB_SET:
            this->value.uset = oth.value.uset;
            break;
        default:
            break;
    }
}
void SDBObject::free() {
    --(*this->count);
    if (*this->count != 0) return;
    if (this->enctype != SdbEncType::SDB_ENC_INT) {
        switch (this->objtype) {
            case SdbObjType::SDB_STRING:
                if (value.str) delete value.str;
                break;
            case SdbObjType::SDB_HASH:
                if (value.dict) delete value.dict;
                break;
            case SdbObjType::SDB_LIST:
                if (value.list) delete value.list;
                break;
            case SdbObjType::SDB_OSET:
                if (value.oset) delete value.oset;
                break;
            case SdbObjType::SDB_SET:
                if (value.uset) delete value.uset;
                break;
            default:
                break;
        }
    }
    delete this->count;
}
SDBObject& SDBObject::operator=(const SDBObject& oth) {
    if (&oth == this) {
        return *this;
    }
    ++(*oth.count);
    this->free();
    this->objtype = oth.objtype;
    this->enctype = oth.enctype;
    this->count = oth.count;
    if (this->enctype == SdbEncType::SDB_ENC_INT) {
        this->value.istr = oth.value.istr;
        return *this;
    }
    switch (this->objtype) {
        case SdbObjType::SDB_STRING:
            this->value.str = oth.value.str;
            break;
        case SdbObjType::SDB_HASH:
            this->value.dict = oth.value.dict;
            break;
        case SdbObjType::SDB_LIST:
            this->value.list = oth.value.list;
            break;
        case SdbObjType::SDB_OSET:
            this->value.oset = oth.value.oset;
            break;
        case SdbObjType::SDB_SET:
            this->value.uset = oth.value.uset;
            break;
        default:
            break;
    }
    return *this;
}
SDBObject::~SDBObject() { this->free(); }
std::shared_ptr<SDBObject> SDBObject::CreateStrObject() {
    return std::make_shared<SDBObject>();
}

std::shared_ptr<SDBObject> SDBObject::CreateListObject() {
    std::shared_ptr<SDBObject> temp = std::make_shared<SDBObject>();
    temp->enctype = SdbEncType::SDB_ENC_LINKEDLIST;
    temp->objtype = SdbObjType::SDB_LIST;
    temp->value.list = new List<std::string>;
    return temp;
}
std::shared_ptr<SDBObject> SDBObject::CreateSetObject() {
    auto temp = std::make_shared<SDBObject>();
    temp->enctype = SdbEncType::SDB_ENC_HT;
    temp->objtype = SdbObjType::SDB_SET;
    temp->value.uset = new Set<std::string>;
    return temp;
}
std::shared_ptr<SDBObject> SDBObject::CreateHashObject() {
    std::shared_ptr<SDBObject> temp = std::make_shared<SDBObject>();
    temp->enctype = SdbEncType::SDB_ENC_HT;
    temp->objtype = SdbObjType::SDB_HASH;
    temp->value.dict = new Dict<std::string>;
    return temp;
}

std::shared_ptr<SDBObject> SDBObject::CreateOSetObject() {
    std::shared_ptr<SDBObject> temp = std::make_shared<SDBObject>();
    temp->enctype = SdbEncType::SDB_ENC_SKIPLIST;
    temp->objtype = SdbObjType::SDB_OSET;
    temp->value.oset = new OrderedSet<std::string>;
    return temp;
}

std::string SDBObject::getObjType() {
    std::string res;
    switch (this->objtype) {
        case SdbObjType::SDB_STRING:
            res = "string";
            break;
        case SdbObjType::SDB_HASH:
            res = "hash";
            break;
        case SdbObjType::SDB_SET:
            res = "set";
            break;
        case SdbObjType::SDB_NULL:
            res = "null";
            break;
        case SdbObjType::SDB_OSET:
            res = "ordered set";
            break;
        case SdbObjType::SDB_LIST:
            res = "list";
            break;
    }
    return res;
}

std::string SDBObject::getEncType() {
    std::string res;
    switch (this->enctype) {
        case SdbEncType::SDB_ENC_INT:
            res = "int";
            break;
        case SdbEncType::SDB_ENC_RAW:
            res = "raw";
            break;
        case SdbEncType::SDB_ENC_INTSET:
            res = "intset";
            break;
        case SdbEncType::SDB_ENC_LINKEDLIST:
            res = "linkedlist";
            break;
        case SdbEncType::SDB_ENC_SKIPLIST:
            res = "skiplist";
            break;
        case SdbEncType::SDB_ENC_HT:
            res = "hashtable";
            break;
    }
    return res;
}

void SDBObject::set(std::string&& s) {
    if (this->objtype != SdbObjType::SDB_STRING &&
        this->objtype != SdbObjType::SDB_NULL) {
        throw SdbException("command is not supported for this type!");
    }

    this->objtype = SdbObjType::SDB_STRING;

    bool isNumber = true;
    bool isNeg = false;
    int64_t res = 0;
    if (s.size() <= 18) {
        unsigned int i = 0;
        if (s[0] == '-') {
            i++;
            isNeg = true;
        }
        for (; i < s.size(); ++i) {
            if (s[i] >= '0' && s[i] <= '9') {
                res = res * 10 + s[i] - '0';
            } else {
                isNumber = false;
                break;
            }
        }
        if (isNumber && isNeg) {
            res = 0 - res;
        }
    }
    if (this->enctype == SdbEncType::SDB_ENC_RAW) {
        if (this->value.str) {
            delete this->value.str;
        }
    }
    if (isNumber) {
        this->value.istr = res;
        this->enctype = SdbEncType::SDB_ENC_INT;
    } else {
        this->value.str = new std::string(std::move(s));
        this->enctype = SdbEncType::SDB_ENC_RAW;
    }
}

void SDBObject::append(const std::string& data) {
    if (this->objtype == SdbObjType::SDB_STRING ||
        this->objtype == SdbObjType::SDB_NULL) {
        if (this->objtype == SdbObjType::SDB_NULL) {
            this->objtype = SdbObjType::SDB_STRING;
            this->value.str = new std::string;
        } else if (this->enctype == SdbEncType::SDB_ENC_INT) {
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
        throw SdbException("command is not supported for this type!");
    }
}

ssize_t SDBObject::getStrLen() {
    if (this->objtype == SdbObjType::SDB_STRING) {
        if (this->enctype == SdbEncType::SDB_ENC_RAW) {
            return this->value.str->size();
        } else {
            int64_t temp = this->value.istr;
            int64_t length = 0;
            if (temp < 0) {
                temp = 0 - temp;
                ++length;
            }
            while (temp) {
                ++length;
                temp /= 10;
            }
            return length;
        }
    } else {
        // error
        throw SdbException("command is not supported for this type!");
    }
}
void SDBObject::push(std::string&& s) { this->value.list->push(std::move(s)); }
void SDBObject::pop(std::ostream& out) {
    if (this->objtype != SdbObjType::SDB_LIST) {
        throw SdbException("command is not supported for this type!");
    }
    if (this->value.list == nullptr) {
        out << "null" << '\n';
    }
    auto temp = this->value.list->pop();
    out << temp->data << '\n';
}
ssize_t SDBObject::llen() {
    if (this->objtype != SdbObjType::SDB_LIST) {
        throw SdbException("command is not supported for this type!");
    }
    if (this->value.list == nullptr) {
        return 0;
    }
    return this->value.list->len();
}
void SDBObject::hadd(std::string&& key, std::string&& value) {
    if (this->objtype != SdbObjType::SDB_HASH) {
        throw SdbException("command is not supported for this type!");
    }
    auto& temp = this->value.dict->operator[](std::move(key));
    temp = std::move(value);
}
void SDBObject::hdel(const std::string& key) { this->value.dict->del(key); }

void SDBObject::oadd(const double& score, std::string&& value) {
    if (this->objtype != SdbObjType::SDB_OSET) {
        throw SdbException("command is not supported for this type!");
    }
    this->value.oset->insert(score, std::move(value));
}
void SDBObject::odel(const double& score) {
    if (this->objtype != SdbObjType::SDB_OSET) {
        throw SdbException("command is not supported for this type!");
    }
    this->value.oset->del(score);
}

void SDBObject::sadd(std::string&& str) {
    if (this->objtype != SdbObjType::SDB_SET) {
        throw SdbException("command is not supported for this type!");
    }
    this->value.uset->add(std::move(str));
}

void SDBObject::sdel(const std::string& str) {
    if (this->objtype != SdbObjType::SDB_SET) {
        throw SdbException("command is not supported for this type!");
    }
    this->value.uset->del(str);
}

void SDBObject::print(std::ostream& out) const {
    if (this->objtype == SdbObjType::SDB_STRING) {
        if (this->enctype == SdbEncType::SDB_ENC_INT) {
            out << std::to_string(this->value.istr) << '\n';
        } else {
            out << *(this->value.str) << '\n';
        }
    } else if (this->objtype == SdbObjType::SDB_LIST) {
        this->value.list->print(out);
    } else if (this->objtype == SdbObjType::SDB_HASH) {
        this->value.dict->print(out);
    } else if (this->objtype == SdbObjType::SDB_OSET) {
        this->value.oset->print(out);
    } else if (this->objtype == SdbObjType::SDB_SET) {
        this->value.uset->print(out);
    } else {
        throw SdbException("unknown type!");
    }
}

}  // namespace SDB

#endif