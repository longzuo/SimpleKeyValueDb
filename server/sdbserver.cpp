#include <iostream>
#include "SdbServer.hpp"
using namespace Net;
using namespace SDB;
int main() {
    InetAddress address = InetAddress::IPV4AddressAny();
    address.setPort(5481);
    SdbServer sdbserver(address);
    sdbserver.run();
}