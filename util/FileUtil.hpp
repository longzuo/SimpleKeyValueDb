#ifndef SDB_FILEUTIL_HPP
#define SDB_FILEUTIL_HPP
#include <fstream>
#include "../core/Exception.hpp"
namespace SDB {
// declare save
enum class IntegerEncoding { INTENC8 = 12, INTENC16, INTENC32, INTENC64 };
IntegerEncoding getIntegerEncoding(unsigned long data);
int getIntegerStorageSize(IntegerEncoding);
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
IntegerEncoding getIntegerEncoding(unsigned long data) {
    if (data <= UINT8_MAX) {
        return IntegerEncoding::INTENC8;
    } else if (data <= UINT16_MAX) {
        return IntegerEncoding::INTENC16;
    } else if (data <= UINT32_MAX) {
        return IntegerEncoding::INTENC32;
    } else {
        return IntegerEncoding::INTENC64;
    }
}
int getIntegerStorageSize(IntegerEncoding enc) {
    switch (enc) {
        case IntegerEncoding::INTENC8:
            return 1;
            break;
        case IntegerEncoding::INTENC16:
            return 2;
            break;
        case IntegerEncoding::INTENC32:
            return 4;
            break;
        case IntegerEncoding::INTENC64:
            return 8;
            break;
        default:
            throw SdbException("unknown integer encoding!");
    }
}
void saveString(std::ofstream& ofs, const std::string& key) {
    size_t len = key.size();
    // ofs.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
    saveInteger(ofs, len);
    ofs << key;
}
void saveInteger(std::ofstream& ofs, const unsigned long data) {
    IntegerEncoding ienc = getIntegerEncoding(data);
    int isize = getIntegerStorageSize(ienc);
    ofs.write(reinterpret_cast<const char*>(&ienc), sizeof(char));
    // ofs.write(reinterpret_cast<const char*>(&data), sizeof(long));
    uint8_t temp8;
    uint16_t temp16;
    uint32_t temp32;
    uint64_t temp64;
    switch (isize) {
        case 1:
            temp8 = static_cast<uint8_t>(data);
            ofs.write(reinterpret_cast<const char*>(&temp8), 1);
            break;
        case 2:
            temp16 = static_cast<uint16_t>(data);
            ofs.write(reinterpret_cast<const char*>(&temp16), 2);
            break;
        case 4:
            temp32 = static_cast<uint32_t>(data);
            ofs.write(reinterpret_cast<const char*>(&temp32), 4);
            break;
        case 8:
            temp64 = static_cast<uint64_t>(data);
            ofs.write(reinterpret_cast<const char*>(&temp64), 8);
            break;
        default:
            throw SdbException(
                "save integer error,encoding size is not correct!");
            break;
    }
}
void saveInteger(std::ofstream& ofs, const unsigned int data) {
    saveInteger(ofs, static_cast<const unsigned long>(data));
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
    long temp = 0;
    loadInteger(ifs, temp);
    idata = static_cast<int>(temp);
}
void loadInteger(std::ifstream& ifs, long& ldata) {
    IntegerEncoding ienc = IntegerEncoding::INTENC64;
    ifs.read(reinterpret_cast<char*>(&ienc), sizeof(char));
    uint8_t temp8;
    uint16_t temp16;
    uint32_t temp32;
    uint64_t temp64;
    switch (ienc) {
        case IntegerEncoding::INTENC8:
            temp8 = 0;
            ifs.read(reinterpret_cast<char*>(&temp8), sizeof(uint8_t));
            ldata = temp8;
            break;
        case IntegerEncoding::INTENC16:
            temp16 = 0;
            ifs.read(reinterpret_cast<char*>(&temp16), sizeof(uint16_t));
            ldata = temp16;
            break;
        case IntegerEncoding::INTENC32:
            temp32 = 0;
            ifs.read(reinterpret_cast<char*>(&temp32), sizeof(uint32_t));
            ldata = temp32;
            break;
        case IntegerEncoding::INTENC64:
            temp64 = 0;
            ifs.read(reinterpret_cast<char*>(&temp64), sizeof(uint64_t));
            ldata = temp64;
            break;
        default:
            throw SdbException("load integer error,unknown encoding type!");
            break;
    }
}
void loadString(std::ifstream& ifs, std::string& str, long len) {
    if (len <= 0) return;
    str.reserve(str.size() + len);
    char ch;
    while (len > 0 && ifs.get(ch)) {
        str.push_back(ch);
        len--;
    }
}
}  // namespace SDB
#endif