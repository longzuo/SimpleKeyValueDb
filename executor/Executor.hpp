#ifndef SDB_EXECUTOR_HPP
#define SDB_EXECUTOR_HPP
#include <sstream>
#include "./DoDb.hpp"
#include "./DoHash.hpp"
#include "./DoList.hpp"
#include "./DoObject.hpp"
#include "./DoOset.hpp"
#include "./DoSet.hpp"
#include "./DoString.hpp"
namespace SDB {

class Executor {
   private:
    std::vector<Db> dblist;
    unsigned int currentdb = 0;

   public:
    Executor() { dblist.resize(2); }
    void execute(const std::string&, std::ostream&);
};
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
    if (words[0] == "object") {
        doObject(words, out, dblist[currentdb]);
    } else if (words[0] == "select" || words[0] == "del" ||
               words[0] == "expire" || words[0] == "pexpire" ||
               words[0] == "save"||words[0]=="load") {
        doDb(words, out, dblist, currentdb);
    } else if (words[0] == "lpush" || words[0] == "lgetall" ||
               words[0] == "lpop" || words[0] == "llen") {
        doList(words, out, dblist[currentdb]);
    } else if (words[0] == "hadd" || words[0] == "hdel" ||
               words[0] == "hgetall") {
        doHash(words, out, dblist[currentdb]);
    } else if (words[0] == "oadd" || words[0] == "odel" ||
               words[0] == "ogetall") {
        doOset(words, out, dblist[currentdb]);
    } else if (words[0] == "sadd" || words[0] == "sdel" ||
               words[0] == "sgetall") {
        doSet(words, out, dblist[currentdb]);
    } else {
        doString(words, out, dblist[currentdb]);
    }
}

}  // namespace SDB
#endif