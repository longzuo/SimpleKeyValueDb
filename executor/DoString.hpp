#ifndef SDB_DOSTRING_HPP
#define SDB_DOSTRING_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doString(std::vector<std::string>&, std::ostream&, Db&);
void doSet(std::vector<std::string>&, std::ostream&, Db&);
void doGet(std::vector<std::string>&, std::ostream&, Db&);
void doStrlen(std::vector<std::string>&, std::ostream&, Db&);
void doAppend(std::vector<std::string>&, std::ostream&, Db&);

// define

void doString(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands[0] == "set") {
        doSet(commands, out, db);
    } else if (commands[0] == "get") {
        doGet(commands, out, db);
    } else if (commands[0] == "append") {
        doAppend(commands, out, db);
    } else if (commands[0] == "strlen") {
        doStrlen(commands, out, db);
    } else {
        throw SdbException("unknown command:" + commands[0]);
    }
}

void doSet(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("error:missing arguments!");
    } else {
        SDBObject::ObjPointer& newptr = db[commands[1]];
        if (!newptr.get()) {
            newptr = SDBObject::CreateStrObject();
        }
        newptr->set(std::move(commands[2]));
    }
}

void doGet(std::vector<std::string>& commands, std::ostream& out, Db& db) {
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
        SDBObject::ObjPointer& res = db[commands[1]];
        if (!res.get()) {
            res = SDBObject::CreateStrObject();
        }
        res->append(commands[2]);
    }
}

void doStrlen(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    SDBObject::ObjPointer res = db.find(commands[1]);
    if (res.get()) {
        out << res->getStrLen() << '\n';
    } else {
        out << "0" << '\n';
    }
}

}  // namespace SDB
#endif