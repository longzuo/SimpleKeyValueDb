#ifndef SDB_CLIENT_HPP
#define SDB_CLIENT_HPP
#include <iostream>
#include "../core/Exception.hpp"
#include "../net/TcpSocket.hpp"
namespace SDB {

class SdbClient {
   private:
    Net::TcpSocket tcpsocket;

   public:
    SdbClient(Net::InetAddress&);
    void send(const std::string&);
    std::string&& read();
};

SdbClient::SdbClient(Net::InetAddress& addr) {
    if (!tcpsocket.connect(addr)) {
        throw SdbException("can not connect to server!");
    }
}

void SdbClient::send(const std::string& msg) {
    if (msg.size() == 0) {
        return;
    }
    size_t total = 0;
    size_t send = 0;
    size_t start = 0;
    std::string msgheader;
    msgheader.append("sdb");
    total += msgheader.size() + msg.size() + sizeof(total);
    msgheader.append(reinterpret_cast<const char*>(&total), sizeof(total));
    total = msgheader.size();
    char* pos = const_cast<char*>(msgheader.c_str());
    while (total > 0) {
        // send 表示每一次写入的字节数
        send = tcpsocket.write(pos + start, total);
        total -= send;
        // start 表示已经写入的字节数，也是下一次写入开始的地址
        start += send;
    }
    total = msg.size();
    send = 0;
    pos = const_cast<char*>(msg.c_str());
    start = 0;
    while (total > 0) {
        send = tcpsocket.write(pos + start, total);
        total -= send;
        start += send;
    }
}

std::string&& SdbClient::read() {
    char buf[512];
    std::string res;
    size_t bytes;
    size_t received = 0;
    bool getlen = false;
    size_t toreceive = 0;
    while ((bytes = tcpsocket.read(buf, 512)) > 0) {
        res.append(buf, bytes);
        received += bytes;
        if (received >= (sizeof(size_t) + 3)) {
            char* pos = reinterpret_cast<char*>(&toreceive);
            for (unsigned int i = 0; i < sizeof(size_t); ++i) {
                pos[i] = res[3 + i];
            }
            getlen = true;
        }
        if (getlen && received >= toreceive) {
            break;
        }
    }
    std::string temp(res, sizeof(size_t) + 3);
    return std::move(temp);
}

}  // namespace SDB

#endif