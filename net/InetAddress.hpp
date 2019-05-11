#ifndef NET_NETADDRESS_HPP
#define NET_NETADDRESS_HPP
#include <string.h>
#include <string>
#include <vector>
#include "NetLib.hpp"
namespace Net {

class InetAddress {
    friend class Socket;
    friend class TcpSocket;

   private:
    union {
        struct sockaddr_in addr4;
        struct sockaddr_in6 addr6;
    } addr;
    NetDomain dm;
    NetSocketType socktype;
    int protocol;

   public:
    InetAddress();
    InetAddress(int, NetDomain);
    InetAddress(struct sockaddr_in&);
    InetAddress(struct sockaddr_in6&);
    static InetAddress IPV6AddressAny();
    static InetAddress IPV4AddressAny();
    static std::vector<std::string> getHostIPAddresses(const std::string&,
                                                       Net::NetDomain);
    static std::vector<InetAddress> getLocalAddresses(NetDomain);
    static std::vector<InetAddress> getHostAddresses(const std::string&);
    static std::string localHostName();
    InetAddress& operator=(const InetAddress&);
    InetAddress(const InetAddress&);
    void setPort(const uint16_t port_num);
    std::string getIp() const;
    int getPort() const;
    NetDomain getDomain() const { return dm; }
    NetSocketType getSocketType() const { return socktype; }
    int getProtocol() const { return protocol; }
};

InetAddress::InetAddress() {
    ::memset((void*)&addr, 0, sizeof(addr));
    dm = NetDomain::NetInet;
    socktype = NetSocketType::NetSocketStream;
    protocol = 0;
}

InetAddress::InetAddress(int port, NetDomain domain = NetDomain::NetInet) {
    ::memset((void*)&addr, 0, sizeof(addr));
    if (domain == NetDomain::NetInet) {
        addr.addr4.sin_family = AF_INET;
        addr.addr4.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.addr4.sin_port = ::htons(port);
    } else {
        addr.addr6.sin6_family = AF_INET6;
        addr.addr6.sin6_addr = in6addr_any;
        addr.addr6.sin6_port = ::htons(port);
    }
    dm = domain;
    socktype = NetSocketType::NetSocketStream;
    protocol = 0;
}

InetAddress::InetAddress(struct sockaddr_in& addr4) {
    ::memcpy((void*)&this->addr.addr4, (void*)&addr4,
             sizeof(struct sockaddr_in));
    dm = NetDomain::NetInet;
    socktype = NetSocketType::NetSocketStream;
    protocol = 0;
}

InetAddress::InetAddress(struct sockaddr_in6& addr6) {
    ::memcpy((void*)&this->addr.addr6, (void*)&addr6,
             sizeof(struct sockaddr_in6));
    dm = NetDomain::NetInet6;
    socktype = NetSocketType::NetSocketStream;
    protocol = 0;
}

InetAddress InetAddress::IPV6AddressAny() {
    return InetAddress(0, NetDomain::NetInet6);
}

InetAddress InetAddress::IPV4AddressAny() {
    return InetAddress(0, NetDomain::NetInet);
}

InetAddress& InetAddress::operator=(const InetAddress& lv) {
    ::memcpy((void*)&this->addr, (void*)&lv.addr, sizeof(addr));
    dm = lv.dm;
}

InetAddress::InetAddress(const InetAddress& lv) {
    ::memcpy((void*)&this->addr, (void*)&lv.addr, sizeof(addr));
    dm = lv.dm;
}

void InetAddress::setPort(const uint16_t port_num) {
    if (dm == NetDomain::NetInet) {
        addr.addr4.sin_port = ::htons(port_num);
    } else {
        addr.addr6.sin6_port = ::htons(port_num);
    }
}

std::vector<std::string> InetAddress::getHostIPAddresses(
    const std::string& hostname, NetDomain domain = NetDomain::NetUnspecific) {
    char hname[ADDRSTRLEN];
    std::vector<std::string> res;
    struct addrinfo hints, *resinfo;
    if (hostname.size() == 0) {
        return res;
    }
    ::bzero(&hints, sizeof(struct addrinfo));
    if (domain == NetDomain::NetInet) {
        hints.ai_family = AF_INET;
    } else if (domain == NetDomain::NetInet6) {
        hints.ai_family = AF_INET6;
    } else {
        hints.ai_family = AF_UNSPEC;
    }
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    if (::getaddrinfo(hostname.c_str(), NULL, &hints, &resinfo) != 0) {
        return res;
    }
    char str[ADDRSTRLEN];
    auto ptr = resinfo;
    while (resinfo != nullptr) {
        if (resinfo->ai_family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*)resinfo->ai_addr;
            if (::inet_ntop(resinfo->ai_family, &addr->sin_addr, str,
                            ADDRSTRLEN)) {
                res.push_back(std::string(str));
            }
        } else {
            struct sockaddr_in6* addr6 = (struct sockaddr_in6*)resinfo->ai_addr;
            if (::inet_ntop(resinfo->ai_family, &addr6->sin6_addr, str,
                            ADDRSTRLEN)) {
                res.push_back(std::string(str));
            }
        }
        resinfo = resinfo->ai_next;
    }
    ::freeaddrinfo(ptr);
    return res;
}

std::vector<InetAddress> InetAddress::getHostAddresses(
    const std::string& hostname) {
    char hname[ADDRSTRLEN];
    std::vector<InetAddress> res;
    res.reserve(10);
    struct addrinfo hints, *resinfo;
    if (hostname.size() == 0) {
        return res;
    }
    ::bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;
    if (::getaddrinfo(hostname.c_str(), NULL, &hints, &resinfo) != 0) {
        return res;
    }
    char str[ADDRSTRLEN];
    auto ptr = resinfo;
    while (resinfo != nullptr) {
        NetSocketType type = static_cast<NetSocketType>(resinfo->ai_socktype);
        if (resinfo->ai_family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*)resinfo->ai_addr;
            InetAddress tmadd(*addr);
            tmadd.protocol = resinfo->ai_protocol;
            tmadd.socktype = static_cast<NetSocketType>(resinfo->ai_socktype);
            res.push_back(tmadd);
        } else {
            struct sockaddr_in6* addr6 = (struct sockaddr_in6*)resinfo->ai_addr;
            InetAddress tmadd(*addr6);
            tmadd.protocol = resinfo->ai_protocol;
            tmadd.socktype = static_cast<NetSocketType>(resinfo->ai_socktype);
            res.push_back(tmadd);
        }
        resinfo = resinfo->ai_next;
    }
    ::freeaddrinfo(ptr);
    return res;
}

std::vector<InetAddress> InetAddress::getLocalAddresses(
    NetDomain domain = NetDomain::NetUnspecific) {
    std::vector<InetAddress> res;
    struct ifaddrs *ifap, *temp;
    char str[ADDRSTRLEN];
    if (::getifaddrs(&ifap) != 0) {
        return res;
    }
    int family = 0;
    if (domain == NetDomain::NetInet) {
        family = AF_INET;
    } else if (domain == NetDomain::NetInet6) {
        family = AF_INET6;
    } else {
        family = AF_UNSPEC;
    }
    for (temp = ifap; temp != nullptr; temp = temp->ifa_next) {
        if (family == AF_UNSPEC || temp->ifa_addr->sa_family == family) {
            if (temp->ifa_addr->sa_family == AF_INET6) {
                struct sockaddr_in6* addr6 =
                    (struct sockaddr_in6*)temp->ifa_addr;
                res.push_back(InetAddress(*addr6));
            } else if (temp->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in* addr4 = (struct sockaddr_in*)temp->ifa_addr;
                res.push_back(InetAddress(*addr4));
            }
        }
    }
    ::freeifaddrs(ifap);
    return res;
}

std::string InetAddress::localHostName() {
    char hostname[ADDRSTRLEN];
    if (::gethostname(hostname, ADDRSTRLEN) != 0) {
        return "";
    }
    return std::string(hostname);
}

std::string InetAddress::getIp() const {
    if (dm == NetDomain::NetInet) {
        return std::string(::inet_ntoa(*(struct in_addr*)&addr.addr4.sin_addr));
    } else {
        char str[ADDRSTRLEN];
        if (::inet_ntop(addr.addr6.sin6_family, &addr.addr6.sin6_addr, str,
                        ADDRSTRLEN)) {
            return std::string(str);
        } else {
            return std::string("");
        }
    }
}

int InetAddress::getPort() const {
    // return port
    if (dm == NetDomain::NetInet) {
        return ::ntohs(addr.addr4.sin_port);
    } else {
        return ::ntohs(addr.addr6.sin6_port);
    }
}

}  // namespace Net

#endif