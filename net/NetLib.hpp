#ifndef NET_NETLIB_HPP
#define NET_NETLIB_HPP
#include <exception>
#include "unix_header.h"
namespace Net {

static const int ADDRSTRLEN = 256;

// enum class NetIPType { NetIPV4 = 1, NetIPV6 = 2 };

enum class NetDomain {
    NetUnix = AF_UNIX,
    NetInet = AF_INET,
    NetInet6 = AF_INET6,
    NetUnspecific = AF_UNSPEC
};

enum class NetSocketType {
    NetSocketStream = SOCK_STREAM,
    NetSocketDgram = SOCK_DGRAM
};

class NetException : public std::runtime_error {
   public:
    explicit NetException(const std::string& msg) : std::runtime_error(msg) {}
};

}  // namespace Net
#endif