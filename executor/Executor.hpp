#ifndef SDB_EXECUTOR_HPP
#define SDB_EXECUTOR_HPP
#include <sstream>
#include "./DoString.hpp"
#include"./DoObject.hpp"
#include"./DoDb.hpp"
namespace SDB {

class Executor {
   private:
    std::vector<Db> dblist;
    int currentdb = 0;

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
        doObject(words,out,dblist[currentdb]);
    } else if (words[0] == "select"||words[0]=="del") {
        doDb(words, out,dblist,currentdb);
    } else {
        doString(words, out,dblist[currentdb]);
    }
}

}  // namespace SDB
#endif