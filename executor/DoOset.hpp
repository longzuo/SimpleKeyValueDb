#ifndef SDB_DOOSET_HPP
#define SDB_DOOSET_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
#include "../util/StringUtil.hpp"
namespace SDB {
// declare
void doOadd(std::vector<std::string>&, std::ostream&, Db&);
void doOdel(std::vector<std::string>&, std::ostream&, Db&);
void doOgetall(std::vector<std::string>&, std::ostream&, Db&);
// define
void doOadd(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 4) {
        throw SdbException("missing arguments!");
    }
    SDBObject::ObjPointer fres = db.find(commands[1]);
    double score = StringUtil::toDouble(commands[2]);
    if (fres.get()) {
        fres->oadd(score, std::move(commands[3]));
        return;
    }
    auto& ptr = db[std::move(commands[1])];
    if (!ptr.get()) {
        ptr = SDBObject::CreateOSetObject();
    }
    ptr->oadd(score, std::move(commands[3]));
}
void doOdel(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("missing arguments!");
    }
    auto ptr = db.find(commands[1]);
    if (ptr.get()) {
        ptr->odel(StringUtil::toDouble(commands[2]));
    } else {
        out << "null" << '\n';
    }
}
void doOgetall(std::vector<std::string>& commands, std::ostream& out, Db& db) {
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