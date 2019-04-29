#ifndef SDB_EXCEPTIONS_HPP
#define SDB_EXCEPTIONS_HPP
#include <stdexcept>
namespace SDB {

class SdbException : public std::runtime_error {
   public:
    explicit SdbException(const std::string& msg) : std::runtime_error(msg) {}
};

}  // namespace SDB
#endif