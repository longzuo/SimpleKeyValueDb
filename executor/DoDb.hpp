#ifndef SDB_DODB_HPP
#define SDB_DODB_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doDb(const std::vector<std::string>&, std::ostream&, std::vector<Db>&,
          int&);
void doSelect(const std::vector<std::string>&, std::ostream&, std::vector<Db>&,
              int&);
void doDel(const std::vector<std::string>&, std::ostream&, Db&);
// define
void doDb(const std::vector<std::string>& commands, std::ostream& out,
          std::vector<Db>& dblist, int& currentdb) {
    if (commands.size() < 2) {
        throw SdbException("error:missing arguments!");
    }
    if (commands[0] == "select") {
        doSelect(commands, out, dblist, currentdb);
    } else if (commands[0] == "del") {
        doDel(commands, out, dblist[currentdb]);
    } else {
        throw SdbException("unknown command:" + commands[0]);
    }
}
void doSelect(const std::vector<std::string>& commands, std::ostream&,
              std::vector<Db>& dblist, int& currentdb) {
    int selectdb = 0;
    for (int i = 0; i < commands[1].size(); ++i) {
        if (commands[1][i] >= '0' && commands[1][i] <= '9') {
            selectdb = selectdb * 10 + commands[1][i] - '0';
        } else {
            throw SdbException("error:input is not a number!");
            return;
        }
    }
    if (selectdb == currentdb) {
        return;
    }
    if (selectdb < 10) {
        if (selectdb > dblist.size()) {
            dblist.resize(selectdb);
        }
        currentdb = selectdb;
    } else {
        throw SdbException("error:number of db is too large!");
    }
}
void doDel(const std::vector<std::string>& commands, std::ostream& out,
           Db& db) {
    db.del(commands[1]);
}
}  // namespace SDB
#endif