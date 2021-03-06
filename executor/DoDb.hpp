#ifndef SDB_DODB_HPP
#define SDB_DODB_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doSelect(const std::vector<std::string>&, std::ostream&, std::vector<Db>&,
              unsigned int&);
void doDel(const std::vector<std::string>&, std::ostream&, Db&);
void doExpire(std::vector<std::string>&, std::ostream&, Db&);
void doPrecisionExpire(std::vector<std::string>&, std::ostream&, Db&);
void doSave(const std::vector<std::string>&, std::ostream&, Db&);
void doLoad(const std::vector<std::string>&, std::ostream&, Db&);
// define
void doSelect(const std::vector<std::string>& commands, std::ostream&,
              std::vector<Db>& dblist, unsigned int& currentdb) {
    unsigned int selectdb = 0;
    for (unsigned int i = 0; i < commands[1].size(); ++i) {
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
void doExpire(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if (commands.size() < 3) {
        throw SdbException("error:missing arguments!");
    }
    db.addExpire(commands[1], commands[2]);
}

void doPrecisionExpire(std::vector<std::string>& commands, std::ostream& out,
                       Db& db) {
    if (commands.size() < 3) {
        throw SdbException("error:missing arguments!");
    }
    db.addPrecisionExpire(commands[1], commands[2]);
}

void doSave(const std::vector<std::string>& commands, std::ostream&, Db& db) {
    if (commands.size() < 2) {
        throw SdbException("error:missing arguments!");
    }
    db.save(commands[1]);
}

void doLoad(const std::vector<std::string>& commands, std::ostream&, Db& db) {
    if (commands.size() < 2) {
        throw SdbException("error:missing arguments!");
    }
    db.load(commands[1]);
}

}  // namespace SDB
#endif