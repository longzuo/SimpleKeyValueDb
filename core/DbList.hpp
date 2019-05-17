#ifndef SDB_DBLIST_HPP
#define SDB_DBLIST_HPP
#include <vector>
#include "Db.hpp"
namespace SDB {
class DbList final {
   private:
    static std::vector<Db> dblist;

   public:
    using DbListType = std::vector<Db>;
    using DbListPointer = DbListType*;
    static void init(int);
    static size_t size() { return dblist.size(); }
    // void resize(int);
    static DbListType* getDbList();
};
DbList::DbListType DbList::dblist;
void DbList::init(int size) {
    // resize
    if (dblist.size() == 0) {
        dblist.resize(size);
    }
}

DbList::DbListPointer DbList::getDbList() { return (&dblist); }
}  // namespace SDB
#endif