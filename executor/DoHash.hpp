#ifndef SDB_HASH_HPP
#define SDB_HASH_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doHash(std::vector<std::string>&, std::ostream&, Db&);
void doHadd(std::vector<std::string>&, std::ostream&, Db&);
void doHdel(std::vector<std::string>&, std::ostream&, Db&);
void doHgetall(std::vector<std::string>&, std::ostream&, Db&);
// define
void doHash(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands[0] == "hadd") {
        doHadd(commands, out, db);
    } else if (commands[0] == "hdel") {
        doHdel(commands, out, db);
    } else if (commands[0] == "hgetall") {
        doHgetall(commands, out, db);
    } else {
        throw SdbException("unknown command:" + commands[0]);
    }
}
void doHadd(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 4) {
        throw SdbException("missing arguments!");
    }
    auto& ptr = db[commands[1]];
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