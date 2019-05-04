#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include "./console/Console.hpp"
using namespace SDB;
int main() {
    Console console;
    console.run();
    // std::cout<<sizeof(SDBObject)<<std::endl;
    // std::cout<<sizeof(std::string)<<std::endl;
}