#ifndef SDB_DOOBJECT_HPP
#define SDB_DOOBJECT_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doObject(const std::vector<std::string>&, std::ostream&, Db&);
void doEncoding(const std::vector<std::string>&, std::ostream&, Db&);
void doType(const std::vector<std::string>&, std::ostream&, Db&);

// define
void doObject(const std::vector<std::string>& commands, std::ostream& out,
              Db& db) {
    if (commands.size() < 3) {
        throw SdbException("missing arguments!");
    }
    if (commands[1] == "encoding") {
        doEncoding(commands, out, db);
    } else if (commands[1] == "type") {
        doType(commands, out, db);
    } else {
        throw SdbException("unknown sub command:" + commands[1]);
    }
}

void doEncoding(const std::vector<std::string>& commands, std::ostream& out,
                Db& db) {
    SDBObject::ObjPointer res = db.find(commands[2]);
    if (res.get()) {
        out << res->getEncType() << '\n';
    } else {
        out << "null" << '\n';
    }
}

void doType(const std::vector<std::string>& commands, std::ostream& out,
            Db& db) {
    SDBObject::ObjPointer res = db.find(commands[2]);
    if (res.get()) {
        out << res->getObjType() << '\n';
    } else {
        out << "null" << '\n';
    }
}
}  // namespace SDB
#endif