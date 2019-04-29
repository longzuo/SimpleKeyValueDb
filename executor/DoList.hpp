#ifndef SDB_DOLIST_HPP
#define SDB_DOLIST_HPP
#include "../core/Db.hpp"
#include "../core/Exception.hpp"
namespace SDB {
// declare
void doList(std::vector<std::string>&, std::ostream&, Db&);
void doPush(std::vector<std::string>&, std::ostream&, Db&);
void doPop(std::vector<std::string>&, std::ostream&, Db&);
// define
void doList(std::vector<std::string>& commands, std::ostream& out, Db& db) {
    if(commands[0]=="push"){
        doPush(commands,out,db);
    }else if(commands[0]=="pop"){
        doPop(commands,out,db);
    }else{
        throw SdbException("unknown command:" + commands[0]);
    }
}
void doPush(std::vector<std::string>& commands, std::ostream& out, Db& db){
    if(commands.size()<3){
        throw SdbException("error:missing arguments!");
    }
    SDBObject::ObjPointer& newptr = db[commands[1]];
    if(!newptr.get()){
        newptr=SDBObject::CreateListObject();
    }
    
}
}  // namespace SDB
#endif