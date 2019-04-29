#ifndef SDB_INTSET_HPP
#define SDB_INTSET_HPP
#include <iostream>
#include <vector>
namespace SDB{
//直接存放3个桶(vector)，根据大小存放到不同的桶
class IntSet {
   private:
    std::vector<int16_t> i16set;
    std::vector<int32_t> i32set;
    std::vector<int64_t> i64set;
    //正数负数0都存放在同一个桶中
    //因此需要记录每个通的负数个数
    //查找的时候根据数据大小选择桶进行查找
    //输出的时候则需要先输出每个桶的负数再输出0和正数
    unsigned int i16NegCount = 0;
    unsigned int i32NegCount = 0;
    unsigned int i64NegCount = 0;

   public:
    void del(const int64_t&);
    bool contains(const int64_t&);
    uint32_t size() const {
        return i16set.size() + i32set.size() + i64set.size();
    }
    void print();
    void insert(const int64_t&);
    // INT32_MAX
};
bool IntSet::contains(const int64_t& data) {
    unsigned int i;
    if (data <= INT16_MAX && data >= INT16_MIN) {
        // find in i16set;
        //采用顺序查找，可以改进为二分查找
        for (i = 0; i < i16set.size(); ++i) {
            if (i16set[i] == data) {
                return true;
            } else if (i16set[i] > data) {
                return false;
            }
        }
    } else if (data <= INT32_MAX && data >= INT32_MIN) {
        // find in i32set;
        for (i = 0; i < i32set.size(); ++i) {
            if (i32set[i] == data) {
                return true;
            } else if (i32set[i] > data) {
                return false;
            }
        }
    } else {
        // find in i64set;
        for (i = 0; i < i64set.size(); ++i) {
            if (i64set[i] == data) {
                return true;
            } else if (i64set[i] > data) {
                return false;
            }
        }
    }
    return false;
}
void IntSet::insert(const int64_t& data) {
    if (this->contains(data)) return;
    int i = 0;
    if (data <= INT16_MAX && data >= INT16_MIN) {
        // insert into i16set
        int16_t temp = data;
        i16set.push_back(temp);
        int i = i16set.size() - 2;
        while (i >= 0 && temp < i16set[i]) {
            i16set[i + 1] = i16set[i];
            --i;
        }
        i16set[i + 1] = temp;
        if (data < 0) {
            ++i16NegCount;
        }
    } else if (data <= INT32_MAX && data >= INT32_MIN) {
        // insert into i32set
        int32_t temp = data;
        i32set.push_back(temp);
        int i = i32set.size() - 2;
        while (i >= 0 && temp < i32set[i]) {
            i32set[i + 1] = i32set[i];
            --i;
        }
        i32set[i + 1] = temp;
        if (data < 0) {
            ++i32NegCount;
        }
    } else {
        // insert into i64set;
        int64_t temp = data;
        i64set.push_back(temp);
        int i = i64set.size() - 2;
        while (i >= 0 && temp < i64set[i]) {
            i64set[i + 1] = i64set[i];
            --i;
        }
        i64set[i + 1] = temp;
        if (data < 0) {
            ++i64NegCount;
        }
    }
}
void IntSet::print() {
    unsigned int i = 0;
    for (i = 0; i < i64NegCount; ++i) {
        std::cout << i64set[i] << " ";
    }
    for (i = 0; i < i32NegCount; ++i) {
        std::cout << i32set[i] << " ";
    }
    for (i = 0; i < i16NegCount; ++i) {
        std::cout << i16set[i] << " ";
    }
    for (i = i16NegCount; i < i16set.size(); ++i) {
        std::cout << i16set[i] << " ";
    }
    for (i = i32NegCount; i < i32set.size(); ++i) {
        std::cout << i32set[i] << " ";
    }
    for (i = i64NegCount; i < i64set.size(); ++i) {
        std::cout << i64set[i] << " ";
    }
    std::cout << std::endl;
}
void IntSet::del(const int64_t& data) {
    unsigned int i = 0;
    if (data <= INT16_MAX && data >= INT16_MIN) {
        // find in i16set;
        //采用顺序查找，可以改进为二分查找
        for (; i < i16set.size(); ++i) {
            if (i16set[i] == data) {
                break;
            } else if (i16set[i] > data) {
                return;
            }
        }
        if (i < i16set.size()) {
            i16set.erase(i16set.begin() + i);
            if (data < 0) {
                --i16NegCount;
            }
        }
    } else if (data <= INT32_MAX && data >= INT32_MIN) {
        // find in i32set;
        for (; i < i32set.size(); ++i) {
            if (i32set[i] == data) {
                break;
            } else if (i32set[i] > data) {
                return;
            }
        }
        if (i < i32set.size()) {
            i32set.erase(i32set.begin() + i);
            if (data < 0) {
                --i32NegCount;
            }
        }
    } else {
        // find in i64set;
        for (; i < i64set.size(); ++i) {
            if (i64set[i] == data) {
                break;
            } else if (i64set[i] > data) {
                return;
            }
        }
        if (i < i64set.size()) {
            i64set.erase(i64set.begin() + i);
            if (data < 0) {
                --i64NegCount;
            }
        }
    }
}

}  // namespace

#endif