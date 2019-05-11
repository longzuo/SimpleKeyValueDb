#ifndef SDB_SDBSERVER_HPP
#define SDB_SEBSERVER_HPP
#include <iostream>
#include <sstream>
#include "../core/Exception.hpp"
#include "../executor/Executor.hpp"
#include "../net/TcpServer.hpp"
namespace SDB {

class SdbServer : public Net::TcpServer {
   private:
    Executor executor;

   public:
    virtual void onMessageCallback(std::shared_ptr<Net::TcpConnection>);
    virtual void onCloseCallback(std::shared_ptr<Net::TcpConnection>);
    virtual void onConnectedCallback(std::shared_ptr<Net::TcpConnection>);
    virtual void onWriteCallback(std::shared_ptr<Net::TcpConnection>);

    SdbServer(Net::InetAddress& addr) : TcpServer(addr) {}
};

void SdbServer::onMessageCallback(std::shared_ptr<Net::TcpConnection> conn) {
    if (conn->getReadBuf().size() < (sizeof(size_t) + 3)) {
        return;
    }
    std::string res = conn->getReadBuf().copyBufferAsString(sizeof(size_t) + 3);
    size_t len = 0;
    char* pos = reinterpret_cast<char*>(&len);
    for (unsigned int i = 0; i < sizeof(size_t); ++i) {
        pos[i] = res[3 + i];
    }
    std::cout << "get len:" << len << std::endl;
    if (conn->getReadBuf().size() >= len) {
        res = conn->getReadBuf().readBufferAsString();
        std::string input(res, sizeof(size_t) + 3);
        std::cout << input << std::endl;
        std::stringbuf sbuf;
        std::ostream out(&sbuf);
        try {
            executor.execute(input, out);
        } catch (const SdbException& e) {
            out << e.what() << std::endl;
        }
        std::string reply = "sdb";
        std::string content = sbuf.str();
        std::cout << "size:" << content.size() << "content:" << content
                  << std::endl;
        size_t len = reply.size() + content.size() + sizeof(len);
        reply.append(reinterpret_cast<char*>(&len), sizeof(len));
        reply.append(content);
        std::cout << "reply size:" << reply.size() << "reply:" << reply
                  << std::endl;
        conn->writeToBuf(reply);
        conn->getEvent()->setWriteable();
        modifyEvent(*conn->getEvent());
    }
}
void SdbServer::onCloseCallback(std::shared_ptr<Net::TcpConnection> conn) {
    //
    std::cout << "close:" << conn->getPeerAddress().getIp() + ":"
              << conn->getPeerAddress().getPort() << std::endl;
}
void SdbServer::onConnectedCallback(std::shared_ptr<Net::TcpConnection> conn) {
    //
    std::cout << "connected:" << conn->getPeerAddress().getIp() + ":"
              << conn->getPeerAddress().getPort() << std::endl;
}

void SdbServer::onWriteCallback(std::shared_ptr<Net::TcpConnection> conn) {
    if (conn->getWriteBuf().size() == 0) {
        // conn->shutdown();
        std::cout << conn->getPeerAddress().getIp() << ":"
                  << conn->getPeerAddress().getPort() << " write completed!"
                  << std::endl;
        conn->getEvent()->unsetWriteable();
        modifyEvent(*conn->getEvent());
    }
}

}  // namespace SDB

#endif