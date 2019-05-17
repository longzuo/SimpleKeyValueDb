#ifndef SDB_DOSTRING_HPP
#define SDB_DOSTRING_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doSset(std::vector<std::string>&, std::ostream&, Db&);
void doSget(std::vector<std::string>&, std::ostream&, Db&);
void doStrlen(std::vector<std::string>&, std::ostream&, Db&);
void doAppend(std::vector<std::string>&, std::ostream&, Db&);
void doGetRange(std::vector<std::string>&, std::ostream&, Db&);
// define

void doSset(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("error:missing arguments!");
    } else {
        SDBObject::ObjPointer fres = db.find(commands[1]);
        if (fres.get()) {
            fres->set(std::move(commands[2]));
            return;
        }
        SDBObject::ObjPointer& newptr = db[commands[1]];
        if (!newptr.get()) {
            newptr = SDBObject::CreateStrObject();
        }
        newptr->set(std::move(commands[2]));
    }
}

void doSget(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 2) {
        throw SdbException("error:missing arguments!");
    } else {
        SDBObject::ObjPointer res = db.find(commands[1]);
        if (res.get()) {
            res->print(out);
        } else {
            out << "null" << '\n';
        }
    }
}

void doAppend(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("error:missing arguments!");
    } else {
        SDBObject::ObjPointer fres = db.find(commands[1]);
        if (fres.get()) {
            fres->append(commands[2]);
            return;
        }
        SDBObject::ObjPointer& res = db[std::move(commands[1])];
        if (!res.get()) {
            res = SDBObject::CreateStrObject();
        }
        res->append(commands[2]);
    }
}

void doStrlen(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 2) {
        throw SdbException("error:missing arguments!");
    }
    SDBObject::ObjPointer res = db.find(commands[1]);
    if (res.get()) {
        out << std::to_string(res->getStrLen()) << '\n';
    } else {
        out << "0" << '\n';
    }
}

void doGetRange(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 4) {
        throw SdbException("error:missing arguments!");
    }
    SDBObject::ObjPointer res = db.find(commands[1]);
    if (res.get()) {
        int start = StringUtil::toDouble(commands[2]);
        int end = StringUtil::toDouble(commands[3]);
        std::string temp = res->getRange(start, end);
        if (temp.size() > 0)
            out << temp << '\n';
        else
            out << "\"\"" << '\n';
    } else {
        out << "null" << '\n';
    }
}

}  // namespace SDB
#endif