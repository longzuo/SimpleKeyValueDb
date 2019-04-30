#ifndef SDB_STRINGUTIL_HPP
#define SDB_STRINGUTIL_HPP
#include "../core/Exception.hpp"
namespace SDB {
class StringUtil final {
   public:
    static double toDouble(const std::string&);
};

double StringUtil::toDouble(const std::string& str) {
    if (str.empty()) {
        throw SdbException("generate a double from an empty string!");
    }
    double res = 0;
    int integer = 0;
    bool isNeg = false;
    unsigned int i = 0;
    if (str[0] == '-') {
        if (str.size() == 1) {
            throw SdbException("can not convert to double from:" + str);
        }
        isNeg = true;
        ++i;
    }
    for (; i < str.size() && str[i] != '.'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            integer = integer * 10 + str[i] - '0';
        } else {
            throw SdbException("can not convert to double from:" + str);
        }
    }
    res = integer;
    integer=0;
    unsigned int start=0;
    if (i<str.size()&&str[i] == '.') {
        ++i;
        start=i;
        for (; i < str.size(); ++i) {
            if (str[i] >= '0' && str[i] <= '9') {
                integer = integer * 10 + str[i] - '0';
            } else {
                throw SdbException("can not convert to double from:" + str);
            }
        }        
    }
    if(integer!=0){
        unsigned int count=str.size()-start;
        double temp=integer;
        while(count){
            temp/=10;
            count--;
        }
        res+=temp;
    }
    if(isNeg){
        res=0-res;
    }
    return res;
}

}  // namespace SDB
#endif