#ifndef SDB_SET_HPP
#define SDB_SET_HPP
#include <unordered_set>
namespace SDB {
//无序集合
template <typename Key, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>>
class Set {
   private:
    std::unordered_set<Key, Hash, Equal> uset;
    using iterator=typename std::unordered_set<Key, Hash, Equal>::iterator;
   public:
    void add(const Key&);
    void add(Key&&);
    void del(const Key&);
    ssize_t size() { return uset.size(); }
    void print(std::ostream&);
};
template <typename Key, typename Hash, typename Equal>
void Set<Key, Hash, Equal>::add(const Key& value) {
    uset.insert(value);
}
template <typename Key, typename Hash, typename Equal>
void Set<Key, Hash, Equal>::add(Key&& value) {
    uset.insert(std::move(value));
}
template <typename Key, typename Hash, typename Equal>
void Set<Key, Hash, Equal>::del(const Key& value) {
    uset.erase(value);
}

// print 不具有通用性
template <typename Key, typename Hash, typename Equal>
void Set<Key, Hash, Equal>::print(std::ostream& out) {
    for (auto it = uset.begin(); it != uset.end(); ++it) {
        out<<*it<<'\n';
    }
}

}  // namespace SDB
#endif