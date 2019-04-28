#ifndef SDB_EXECUTOR_HPP
#define SDB_EXECUTOR_HPP
#include <sstream>
#include "../core/Db.hpp"
namespace SDB {

class Executor {
   private:
    std::vector<Db> dblist;
    int currentdb = 0;

   public:
    Executor() { dblist.resize(2); }
    void execute(const std::string&, std::ostream&);
    void doString(std::vector<std::string>&, std::ostream&);
    void doDb(const std::vector<std::string>&, std::ostream&);
    void doObject(const std::vector<std::string>&, std::ostream&);
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
        doObject(words,out);
    } else if (words[0] == "select") {
        doDb(words, out);
    } else {
        doString(words, out);
    }
}

void Executor::doObject(const std::vector<std::string>& commands,
                        std::ostream& out) {
    if (commands[1] == "encoding") {
        SDBObject::ObjPointer res = dblist[currentdb].find(commands[2]);
        if (res.get()) {
            out<<res->getEncType()<<std::endl;
        } else {
            out << "null" << std::endl;
        }
    }else if(commands[1]=="type"){
        SDBObject::ObjPointer res = dblist[currentdb].find(commands[2]);
        if (res.get()) {
            out<<res->getObjType()<<std::endl;
        } else {
            out << "null" << std::endl;
        }
    }else{
        out<<"unknown sub command:"<<commands[1]<<std::endl;
    }
}

void Executor::doString(std::vector<std::string>& commands, std::ostream& out) {
    if (commands[0] == "set") {
        if (commands.size() < 3) {
            out << "error:missing arguments!" << std::endl;
        } else {
            SDBObject::ObjPointer& newptr = dblist[currentdb][commands[1]];
            if(!newptr.get()){
                newptr = SDBObject::CreateStrObject();
            }
            bool isNumber = true;
            bool isNeg = false;
            int64_t res = 0;
            if (commands[2].size() <= 18) {
                int i = 0;
                if (commands[2][0] == '-') {
                    i++;
                    isNeg = true;
                }
                for (; i < commands[2].size(); ++i) {
                    if (commands[2][i] >= '0' && commands[2][i] <= '9') {
                        res = res * 10 + commands[2][i] - '0';
                    } else {
                        isNumber = false;
                        break;
                    }
                }
                if (isNumber && isNeg) {
                    res = 0 - res;
                }
            }
            if (isNumber) {
                newptr->set(res);
            } else {
                newptr->set(std::move(commands[2]));
            }
        }
    } else if (commands[0] == "get") {
        if (commands.size() < 2) {
            out << "error:missing arguments!" << std::endl;
        } else {
            SDBObject::ObjPointer res = dblist[currentdb].find(commands[1]);
            if (res.get()) {
                res->print(out);
            } else {
                out << "null" << std::endl;
            }
        }
    } else if (commands[0] == "append") {
        if (commands.size() < 3) {
            out << "error:missing arguent" << std::endl;
        } else {
            SDBObject::ObjPointer& res = dblist[currentdb][commands[1]];
            if(!res.get()){
                res = SDBObject::CreateStrObject();
            }
            res->append(commands[2]);
        }
    } else if (commands[0] == "strlen") {
        SDBObject::ObjPointer res = dblist[currentdb].find(commands[1]);
        if (res.get()) {
            out << res->getStrLen() << std::endl;
        } else {
            out << "0" << std::endl;
        }
    } else {
        out << "unknown command:" << commands[0] << std::endl;
    }
}

void Executor::doDb(const std::vector<std::string>& commands,
                    std::ostream& out) {
    if (commands.size() < 2) {
        out << "error:missing arguments" << std::endl;
    }
    int selectdb = 0;
    for (int i = 0; i < commands[1].size(); ++i) {
        if (commands[1][i] >= '0' && commands[1][i] <= '9') {
            selectdb = selectdb * 10 + commands[1][i] - '0';
        } else {
            out << "error:input is not number!" << std::endl;
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
        out << "db is too large:" << selectdb << std::endl;
    }
}

}  // namespace SDB
#endif