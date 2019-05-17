#ifndef SDB_EXECUTOR_HPP
#define SDB_EXECUTOR_HPP
#include <functional>
#include <sstream>
#include "./DoDb.hpp"
#include "./DoHash.hpp"
#include "./DoList.hpp"
#include "./DoObject.hpp"
#include "./DoOset.hpp"
#include "./DoSet.hpp"
#include "./DoString.hpp"
namespace SDB {

using DefaultFunc =
    std::function<void(std::vector<std::string>&, std::ostream&, Db&)>;
class Executor {
   private:
    std::vector<Db> dblist;
    unsigned int currentdb = 0;
    std::unordered_map<std::string, DefaultFunc> funcTables;

    void initFuncTables();

   public:
    Executor();
    void execute(const std::string&, std::ostream&);
};
Executor::Executor() {
    dblist.resize(2);
    initFuncTables();
}
void Executor::execute(const std::string& command, std::ostream& out) {
    std::vector<std::string> words;
    std::istringstream wordsStream(command);
    std::string temp;
    while (wordsStream >> temp) {
        words.push_back(std::move(temp));
    }
    if (words.size() == 0) {
        return;
    }
    auto funcIt = funcTables.find(words[0]);
    if (funcIt != funcTables.end()) {
        funcIt->second(words, out, dblist[currentdb]);
    } else if (words[0] == "select") {
        doSelect(words, out, dblist, currentdb);
    } else {
        throw SdbException("unknown command:" + words[0]);
    }
}
void Executor::initFuncTables() {
    funcTables.reserve(40);
    funcTables["object"] = doObject;
    funcTables["lpush"] = doPush;
    funcTables["lgetall"] = doLGetAll;
    funcTables["lpop"] = doPop;
    funcTables["llen"] = doLlen;
    funcTables["hadd"] = doHadd;
    funcTables["hdel"] = doHdel;
    funcTables["hgetall"] = doHgetall;
    funcTables["oadd"] = doOadd;
    funcTables["odel"] = doOdel;
    funcTables["ogetall"] = doOgetall;
    funcTables["sadd"] = doSadd;
    funcTables["sdel"] = doSdel;
    funcTables["sgetall"] = doSgetall;
    funcTables["set"] = doSset;
    funcTables["get"] = doSget;
    funcTables["append"] = doAppend;
    funcTables["strlen"] = doStrlen;
    funcTables["getrange"] = doGetRange;
    funcTables["del"] = doDel;
    funcTables["expire"] = doExpire;
    funcTables["pexpire"] = doPrecisionExpire;
    funcTables["save"] = doSave;
    funcTables["load"] = doLoad;
}

}  // namespace SDB
#endif