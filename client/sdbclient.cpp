#include "SdbClient.hpp"
#include <iostream>
using namespace SDB;
int main() {
    Net::InetAddress addr = Net::InetAddress::IPV4AddressAny();
    addr.setPort(5481);
    SdbClient client(addr);
    std::string input;
    std::string res;
    while (1) {
        std::cout << "sdb>>";
        std::getline(std::cin, input);
        if (input == "exit") {
            std::cout << "bye bye!" << std::endl;
            input.clear();
            //client
            break;
        } else if (input.size() == 0) {
            continue;
        }
        try {
            client.send(input);
            res = client.read();
            std::cout << res;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}