#ifndef NET_TCPSOCKET_HPP
#define NET_TCPSOCKET_HPP
#include "Socket.hpp"
namespace Net {

class TcpSocket : public Socket {
   private:
    InetAddress peerAddress;

   public:
    void setTcpNoDelay(bool);
    bool listen(int);
    int accept(InetAddress&);
    bool connect(InetAddress&);
    bool connect(const std::string&, int);
    size_t read(char*, size_t);
    size_t write(const char*, size_t);
    InetAddress getPeerAddress() const { return peerAddress; }
};

void TcpSocket::setTcpNoDelay(bool enable) {
    int opt = enable ? 1 : 0;
    ::setsockopt(this->getfd(), IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

bool TcpSocket::listen(int backlog = 1) {
    if (this->getfd() == -1) return false;
    return (::listen(this->getfd(), backlog) == 0);
}

int TcpSocket::accept(InetAddress& address) {
    socklen_t len;
    if (getDomain() == NetDomain::NetInet) {
        len = sizeof(address.addr.addr4);
        return ::accept(getfd(), (struct sockaddr*)&address.addr.addr4, &len);
    } else if (getDomain() == NetDomain::NetInet6) {
        len = sizeof(address.addr.addr6);
        return ::accept(getfd(), (struct sockaddr*)&address.addr.addr6, &len);
    } else {
        return -1;
    }
}

bool TcpSocket::connect(InetAddress& address) {
    if (getfd() == -1) {
        if (!create(address.dm, NetSocketType::NetSocketStream,
                    address.protocol)) {
            return false;
        }
    }
    if (address.dm == NetDomain::NetInet) {
        if ((::connect(this->getfd(), (struct sockaddr*)&address.addr.addr4,
                       sizeof(address.addr.addr4)) == 0)) {
            peerAddress = address;
            return true;
        }
    } else if (address.dm == NetDomain::NetInet6) {
        if ((::connect(this->getfd(), (struct sockaddr*)&address.addr.addr6,
                       sizeof(address.addr.addr6)) == 0)) {
            peerAddress = address;
            return true;
        }
    }
    closefd();
    return false;
}

bool TcpSocket::connect(const std::string& dm_ip, int port = 80) {
    auto address = InetAddress::getHostAddresses(dm_ip);
    for (auto it = address.begin(); it != address.end(); ++it) {
        it->setPort(port);
        if (connect(*it)) {
            return true;
        }
    }
    return false;
}

size_t TcpSocket::read(char* buf, size_t s) {
    //
    return Socket::read(buf, s);
}
size_t TcpSocket::write(const char* buf, size_t s) {
    //
    return Socket::write(buf, s);
}

}  // namespace Net
#endif