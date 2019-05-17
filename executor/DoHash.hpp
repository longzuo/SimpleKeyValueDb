#ifndef SDB_HASH_HPP
#define SDB_HASH_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doHadd(std::vector<std::string>&, std::ostream&, Db&);
void doHdel(std::vector<std::string>&, std::ostream&, Db&);
void doHgetall(std::vector<std::string>&, std::ostream&, Db&);
// define
void doHadd(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 4) {
        throw SdbException("missing arguments!");
    }
    SDBObject::ObjPointer fres = db.find(commands[1]);
    if (fres.get()) {
        fres->hadd(std::move(commands[2]), std::move(commands[3]));
        return;
    }
    auto& ptr = db[std::move(commands[1])];
    if (!ptr.get()) {
        ptr = SDBObject::CreateHashObject();
    }
    ptr->hadd(std::move(commands[2]), std::move(commands[3]));
}

void doHdel(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("missing arguments!");
    }
    auto ptr = db.find(commands[1]);
    if (ptr.get()) {
        ptr->hdel(commands[2]);
    } else {
        out << "null" << '\n';
    }
}
void doHgetall(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 2) {
        throw SdbException("missing arguments!");
    }
    auto ptr = db.find(commands[1]);
    if (ptr.get()) {
        ptr->print(out);
    } else {
        out << "null" << '\n';
    }
}
}  // namespace SDB
#endif