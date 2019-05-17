#ifndef SDB_DOLIST_HPP
#define SDB_DOLIST_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doList(std::vector<std::string>&, std::ostream&, Db&);
void doPush(std::vector<std::string>&, std::ostream&, Db&);
void doPop(std::vector<std::string>&, std::ostream&, Db&);
void doLGetAll(std::vector<std::string>&, std::ostream&, Db&);
void doLlen(std::vector<std::string>&, std::ostream&, Db&);
// define
void doList(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands[0] == "lpush") {
        doPush(commands, out, db);
    } else if (commands[0] == "lpop") {
        doPop(commands, out, db);
    } else if (commands[0] == "lgetall") {
        doLGetAll(commands, out, db);
    } else if (commands[0] == "llen") {
        doLlen(commands, out, db);
    } else {
        throw SdbException("unknown command:" + commands[0]);
    }
}
void doPush(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("error:missing arguments!");
    }
    SDBObject::ObjPointer fres = db.find(commands[1]);
    if (fres.get()) {
        fres->push(std::move(commands[2]));
        return;
    }
    SDBObject::ObjPointer& newptr = db[std::move(commands[1])];
    if (!newptr.get()) {
        newptr = SDBObject::CreateListObject();
    }
    newptr->push(std::move(commands[2]));
}
void doPop(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 2) {
        throw SdbException("error:missing arguments!");
    } else {
        SDBObject::ObjPointer res = db.find(commands[1]);
        if (res.get()) {
            res->pop(out);
        } else {
            out << "null" << '\n';
        }
    }
}
void doLGetAll(std::vector<std::string>& commands, std::ostream& out, Db& db) {
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
void doLlen(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 2) {
        throw SdbException("error:missing arguments!");
    } else {
        SDBObject::ObjPointer res = db.find(commands[1]);
        if (res.get()) {
            out << std::to_string(res->llen()) << '\n';
            // out<<res->llen()<<'\n';
        } else {
            out << "0" << '\n';
        }
    }
}
}  // namespace SDB
#endif