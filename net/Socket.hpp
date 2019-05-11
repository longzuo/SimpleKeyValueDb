#ifndef NET_SOCKET_HPP
#define NET_SOCKET_HPP
#include "InetAddress.hpp"
namespace Net {

class Socket {
   private:
    int fd;
    int backlog;
    NetDomain dm;
    bool hasCreated;

   public:
    Socket(int);
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    ~Socket();

    bool create(NetDomain, NetSocketType, int);
    bool bind(InetAddress&);
    int getfd();
    void closefd();
    NetDomain getDomain() const { return dm; }
    size_t read(void*, size_t);
    size_t write(const void*, size_t);
};

Socket::Socket(int _backlog = 1) {
    fd = -1;
    backlog = _backlog;
    dm = NetDomain::NetUnspecific;
    hasCreated = false;
}

bool Socket::create(NetDomain domain,
                    NetSocketType tp = NetSocketType::NetSocketStream,
                    int protocol = 0) {
    fd = socket(static_cast<int>(domain), static_cast<int>(tp), protocol);
    if (fd != -1) {
        hasCreated = true;
        dm = domain;
        return true;
    } else {
        return false;
    }
}

bool Socket::bind(InetAddress& address) {
    if (address.dm == NetDomain::NetInet) {
        this->dm = address.dm;
        return (::bind(fd, (struct sockaddr*)(&address.addr.addr4),
                       sizeof(address.addr.addr4)) == 0);
    } else if (address.dm == NetDomain::NetInet6) {
        this->dm = address.dm;
        return (::bind(fd, (struct sockaddr*)&address.addr.addr6,
                       sizeof(address.addr.addr6)) == 0);
    } else {
        return false;
    }
}

int Socket::getfd() { return fd; }

Socket::~Socket() { this->closefd(); }

void Socket::closefd() {
    if (hasCreated && fd != -1) {
        ::close(fd);
        fd = -1;
        hasCreated = false;
    }
}

size_t Socket::read(void* buf, size_t count) {
    // read
    return ::read(fd, buf, count);
}
size_t Socket::write(const void* buf, size_t count) {
    return ::write(fd, buf, count);
}

}  // namespace Net
#endif