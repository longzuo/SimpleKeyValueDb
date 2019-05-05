#ifndef SDB_FILEUTIL_HPP
#define SDB_FILEUTIL_HPP
#include <fstream>
namespace SDB {
// declare save
void saveInteger(std::ofstream&, const unsigned long);
void saveInteger(std::ofstream&, const unsigned int);
void saveInteger(std::ofstream&, const unsigned short);
void saveInteger(std::ofstream&, const unsigned char);
void saveString(std::ofstream&, const std::string&);
// declare load
void loadInteger(std::ifstream&, char&);
void loadInteger(std::ifstream&, short&);
void loadInteger(std::ifstream&, int&);
void loadInteger(std::ifstream&, long&);
void loadString(std::ifstream&, std::string&, long);
// define save
void saveString(std::ofstream& ofs, const std::string& key) {
    size_t len = key.size();
    // ofs.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
    saveInteger(ofs, len);
    ofs << key;
}
void saveInteger(std::ofstream& ofs, const unsigned long data) {
    ofs.write(reinterpret_cast<const char*>(&data), sizeof(long));
}
void saveInteger(std::ofstream& ofs, const unsigned int data) {
    ofs.write(reinterpret_cast<const char*>(&data), sizeof(int));
}
void saveInteger(std::ofstream& ofs, const unsigned short data) {
    ofs.write(reinterpret_cast<const char*>(&data), sizeof(short));
}
void saveInteger(std::ofstream& ofs, const unsigned char data) {
    ofs.write(reinterpret_cast<const char*>(&data), sizeof(char));
}

// define load
void loadInteger(std::ifstream& ifs, char& cdata) { ifs.get(cdata); }
void loadInteger(std::ifstream& ifs, short& cdata) {
    ifs.read(reinterpret_cast<char*>(&cdata), sizeof(short));
}
void loadInteger(std::ifstream& ifs, int& idata) {
    ifs.read(reinterpret_cast<char*>(&idata), sizeof(int));
}
void loadInteger(std::ifstream& ifs, long& ldata) {
    ifs.read(reinterpret_cast<char*>(&ldata), sizeof(long));
}
void loadString(std::ifstream& ifs, std::string& str, long len) {
    str.reserve(str.size() + len);
    char ch;
    while (len > 0 && ifs.get(ch)) {
        str.push_back(ch);
        len--;
    }
}
}  // namespace SDB
#endif