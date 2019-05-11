#ifndef NET_TCPSERVER_HPP
#define NET_TCPSERVER_HPP
#include <map>
#include <memory>
#include "Epoll.hpp"
#include "TcpConnection.hpp"
#include "TcpSocket.hpp"
namespace Net {

class TcpServer {
   private:
    InetAddress localAddress;
    TcpSocket tcpsocket;
    Epoll epoll;
    std::map<int, std::shared_ptr<TcpConnection>> connections;
    std::map<int, std::shared_ptr<IOEvent>> eventpool;
    EpollEvents activeEvents;
    std::vector<int> fdtoClose;

   public:
    TcpServer(InetAddress&);
    void newConnectionHandle();
    void addNewConnection(int, TcpConnectionPtr&);
    void rmEventConnection();
    void run();
    void notifyClose(std::shared_ptr<TcpConnection>);
    bool addEvent(IOEvent& e) { return epoll.addEvent(e); }
    bool delEvent(IOEvent& e) { return epoll.delEvent(e); }
    bool modifyEvent(IOEvent& e) { return epoll.modifyEvent(e); }

    virtual void onMessageCallback(std::shared_ptr<TcpConnection>) = 0;
    virtual void onCloseCallback(std::shared_ptr<TcpConnection>) = 0;
    virtual void onConnectedCallback(std::shared_ptr<TcpConnection>) = 0;
    virtual void onWriteCallback(std::shared_ptr<TcpConnection>) = 0;
};

TcpServer::TcpServer(InetAddress& addr) {
    if (!tcpsocket.create(NetDomain::NetInet, NetSocketType::NetSocketStream,
                          0)) {
        throw NetException("create socket failed!");
    }
    if (!tcpsocket.bind(addr)) {
        throw NetException("bind socket failed!");
    }
    if (!tcpsocket.listen()) {
        throw NetException("listen socket failed!");
    }
    auto accevent = std::make_shared<IOEvent>(tcpsocket.getfd());
    accevent->setReadFunc(std::bind(&TcpServer::newConnectionHandle, this));
    eventpool.insert({tcpsocket.getfd(), accevent});
    accevent->setReadable();
    addEvent(*accevent);
    activeEvents.reserve(10);
    activeEvents.addEvent();
}

void TcpServer::newConnectionHandle() {
    InetAddress peerAddress;
    int nfd = tcpsocket.accept(peerAddress);
    auto newconnection = std::make_shared<TcpConnection>(nfd, peerAddress);
    newconnection->setOnMessageCallback(
        std::bind(&TcpServer::onMessageCallback, this, std::placeholders::_1));
    newconnection->setOnCloseCallback(
        std::bind(&TcpServer::onCloseCallback, this, std::placeholders::_1));
    newconnection->setOnWriteCallback(
        std::bind(&TcpServer::onWriteCallback, this, std::placeholders::_1));
    newconnection->setNotifyClose(
        std::bind(&TcpServer::notifyClose, this, std::placeholders::_1));
    addNewConnection(nfd, newconnection);
    onConnectedCallback(newconnection);
}

void TcpServer::addNewConnection(int fd, TcpConnectionPtr& ptr) {
    connections.insert({fd, ptr});
    eventpool.insert({fd, ptr->getEvent()});
    ptr->getEvent()->setReadable();
    addEvent(*(ptr->getEvent()));
    activeEvents.addEvent();
}

void TcpServer::notifyClose(std::shared_ptr<TcpConnection> conn) {
    //
    fdtoClose.push_back(conn->getEvent()->getfd());
    delEvent(*conn->getEvent());
    ::close(conn->getEvent()->getfd());
}

void TcpServer::run() {
    while (true) {
        epoll.waitEvents(activeEvents, -1);
        for (int i = 0; i < activeEvents.size(); i++) {
            auto eptr = eventpool.find(activeEvents[i].data.fd);
            if (eptr != eventpool.end()) {
                eptr->second->handleEvent(activeEvents[i].events);
            }
        }
        // close handle
        activeEvents.clear();
        for (unsigned int i = 0; i < fdtoClose.size(); i++) {
            eventpool.erase(fdtoClose[i]);
            connections.erase(fdtoClose[i]);
        }
        fdtoClose.clear();
        // epoll delete
    }
}

}  // namespace Net

#endif