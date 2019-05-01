#ifndef SDB_DOSET_HPP
#define SDB_DOSET_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doSet(std::vector<std::string>&, std::ostream&, Db&);
void doSadd(std::vector<std::string>&, std::ostream&, Db&);
void doSdel(std::vector<std::string>&, std::ostream&, Db&);
void doSgetall(std::vector<std::string>&, std::ostream&, Db&);
// define
void doSet(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands[0] == "sadd") {
        doSadd(commands, out, db);
    } else if (commands[0] == "sdel") {
        doSdel(commands, out, db);
    } else if (commands[0] == "sgetall") {
        doSgetall(commands, out, db);
    }
}

void doSadd(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("missing arguments!");
    }
    auto& ptr = db[std::move(commands[1])];
    if (!ptr.get()) {
        ptr = SDBObject::CreateSetObject();
    }
    ptr->sadd(std::move(commands[2]));
}
void doSdel(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("missing arguments!");
    }
    auto ptr = db.find(commands[1]);
    if (ptr.get()) {
        ptr->sdel(std::move(commands[2]));
    } else {
        out << "null" << '\n';
    }
}
void doSgetall(std::vector<std::string>& commands, std::ostream& out, Db& db) {
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