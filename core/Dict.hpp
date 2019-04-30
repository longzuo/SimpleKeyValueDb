#ifndef SDB_DICT_HPP
#define SDB_DICT_HPP
#include <unordered_map>
namespace SDB {

// unordered_map 不是渐进式rehash，如果有必要可以自定义哈希表数据结构
template <typename Value>
class Dict {
   private:
    std::unordered_map<std::string, Value> umap;

   public:
    ssize_t size() { return umap.size(); }
    void clear() { umap.clear(); }

    void add(const std::string& key, const Value& value);
    void add(std::string&& key, Value&& value);
    void replace(const std::string& key, const Value& value);
    void replace(std::string&& key, Value&& value);
    Value get(const std::string& key) { return umap[key]; }
    void del(const std::string&);
    auto& operator[](const std::string& key);
    auto& operator[](std::string&& key);
    void print(std::ostream&);
};

template <typename Value>
void Dict<Value>::add(const std::string& key, const Value& value) {
    umap.insert({key, value});
}

template <typename Value>
void Dict<Value>::add(std::string&& key, Value&& value) {
    umap.insert(std::move({std::move(key), std::move(value)}));
}

template <typename Value>
void Dict<Value>::replace(const std::string& key, const Value& value) {
    umap[key] = value;
}

template <typename Value>
void Dict<Value>::replace(std::string&& key, Value&& value) {
    umap[std::move(key)] = std::move(value);
}

template <typename Value>
void Dict<Value>::del(const std::string& key) {
    umap.erase(key);
}
template <typename Value>
auto& Dict<Value>::operator[](const std::string& key) {
    return umap[key];
}

template <typename Value>
auto& Dict<Value>::operator[](std::string&& key) {
    return umap[key];
}

template <typename Value>
void Dict<Value>::print(std::ostream& out) {
    for (auto it = umap.begin(); it != umap.end(); ++it) {
        out << it->first << " " << it->second << '\n';
    }
}

}  // namespace SDB

#endif