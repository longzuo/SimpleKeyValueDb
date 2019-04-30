#ifndef SDB_ORDEREDSET_HPP
#define SDB_ORDEREDSET_HPP
#include "./SkipList.hpp"
namespace SDB {
template <typename T>
class OrderedSet {
   private:
    SkipList<T> slist;

   public:
    OrderedSet(const int& _max_level = 32) : slist(_max_level) {}
    typename SkipList<T>::NodePointer insert(const double& score,
                                             const T& value);
    typename SkipList<T>::NodePointer insert(const double& score,
                                            T&& value);
    void del(const double&);
    void print(std::ostream&);
};

template <typename T>
typename SkipList<T>::NodePointer OrderedSet<T>::insert(const double& score,
                                                        const T& value) {
    return slist.insert(score, value);
}
template <typename T>
typename SkipList<T>::NodePointer OrderedSet<T>::insert(const double& score,
                                                        T&& value) {
    return slist.insert(score, std::move(value));
}

template <typename T>
void OrderedSet<T>::del(const double& score) {
    slist.del(score);
}
template <typename T>
void OrderedSet<T>::print(std::ostream& out) {
    this->slist.print(out);
}

}  // namespace SDB
#endif