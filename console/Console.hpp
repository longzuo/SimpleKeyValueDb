#ifndef SDB_CONSOLE_HPP
#define SDB_CONSOLE_HPP
#include <iostream>
#include <string>
#include"../executor/Executor.hpp"
#include"../core/Exception.hpp"
namespace SDB {
class Console {
   private:
    std::string input;
    bool isRunning;
   public:
    void run();
};
void Console::run(){
    Executor executor;
    while(true){
        std::cout<<"sdb>";
        std::getline(std::cin,input);
        if(input=="exit"){
            std::cout<<std::endl;
            std::cout<<"bye"<<std::endl;
            break;
        }
        try{
            executor.execute(input,std::cout);
            std::cout<<std::flush;
        }catch(const SdbException& e){
            std::cout<<e.what()<<std::endl;
        }
    }
}
}  // namespace SDB
#endif