#ifndef NET_TCPCONNECTION_HPP
#define NET_TCPCONNECTION_HPP
#include <memory>
#include "Buffer.hpp"
#include "IOEvent.hpp"
#include "InetAddress.hpp"
namespace Net {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
   public:
    using DefaultCallback = std::function<void(std::shared_ptr<TcpConnection>)>;

   private:
    IOEventPtr event;
    InetAddress peerAddress;
    Buffer readbuf;
    Buffer writebuf;
    DefaultCallback onMessageCallback;
    DefaultCallback onCloseCallback;
    DefaultCallback onErrorCallback;
    DefaultCallback onWriteCallback;
    DefaultCallback notifyClose;

   public:
    TcpConnection(int fd, InetAddress& peer) : peerAddress(peer) {
        event = std::make_shared<IOEvent>(fd);
        event->setReadFunc(std::bind(&TcpConnection::read, this));
        event->setWriteFunc(std::bind(&TcpConnection::write, this));
        event->setCloseFunc(std::bind(&TcpConnection::closeConnection, this));
        event->setReadable();
    }
    void read();
    void write();
    void closeConnection();
    void readFromBuf();
    void writeToBuf(const std::string& msg);
    Buffer& getReadBuf() { return readbuf; }
    Buffer& getWriteBuf() { return writebuf; }
    void setOnMessageCallback(const DefaultCallback&);
    void setOnCloseCallback(const DefaultCallback&);
    void setOnErrorCallback(const DefaultCallback&);
    void setOnWriteCallback(const DefaultCallback&);
    void setNotifyClose(const DefaultCallback&);
    std::string&& readBufferAsString() { return readbuf.readBufferAsString(); }
    InetAddress getPeerAddress() { return peerAddress; }
    IOEventPtr& getEvent() { return event; }
};

void TcpConnection::read() {
    //
    if (readbuf.readFromFd(event->getfd()) > 0) {
        if (onMessageCallback) {
            onMessageCallback(shared_from_this());
        }
    } else {
        if (notifyClose) {
            notifyClose(shared_from_this());
        }
        if (onCloseCallback) {
            onCloseCallback(shared_from_this());
        }
    }
}

void TcpConnection::write() {
    //
    if (writebuf.size() == 0) {
        return;
    }
    if (writebuf.writeToFd(event->getfd()) > 0) {
        if (onWriteCallback) {
            onWriteCallback(shared_from_this());
        }
    } else {
        if (notifyClose) {
            notifyClose(shared_from_this());
        }
        if (onCloseCallback) {
            onCloseCallback(shared_from_this());
        }
    }
}

void TcpConnection::closeConnection() {
    if (notifyClose) {
        notifyClose(shared_from_this());
    }
    if (onCloseCallback) {
        onCloseCallback(shared_from_this());
    }
}

void TcpConnection::writeToBuf(const std::string& msg) {
    // add
    writebuf.push(msg);
}

void TcpConnection::setOnMessageCallback(const DefaultCallback& mc) {
    //
    onMessageCallback = mc;
}

void TcpConnection::setOnCloseCallback(const DefaultCallback& cc) {
    //
    onCloseCallback = cc;
}

void TcpConnection::setOnErrorCallback(const DefaultCallback& ec) {
    //
    onErrorCallback = ec;
}

void TcpConnection::setOnWriteCallback(const DefaultCallback& wc) {
    //
    onWriteCallback = wc;
}

void TcpConnection::setNotifyClose(const DefaultCallback& nc) {
    //
    notifyClose = nc;
}

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

}  // namespace Net
#endif