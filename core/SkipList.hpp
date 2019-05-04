#ifndef SDB_SKIPLIST_HPP
#define SDB_SKIPLIST_HPP
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
namespace SDB {

template <typename T>
class SkipListNode {
   public:
    class Level {
       public:
        //前进指针
        std::shared_ptr<SkipListNode> forward;
        //跨度
        unsigned int span = 0;
    };
    // level s数组
    std::vector<Level> level;
    //后退指针
    std::shared_ptr<SkipListNode> backward;
    //分值
    double score;
    //数据
    T data;
    auto& next(){return level[0].forward;}
};

template <typename T>
class SkipList {
   public:
    using Node = SkipListNode<T>;
    using NodePointer = std::shared_ptr<SkipListNode<T>>;

   private:
    NodePointer head;
    NodePointer tail;
    // std::shared_ptr<SkipListNode<T>> head;
    // std::shared_ptr<SkipListNode<T>> tail;
    //表中节点的数量
    unsigned long length;
    //表中层数最大的节点的层数
    int level;
    int maxLevel = 32;

    int randomLevel();

   public:
    SkipList(const int& _max_level = 32);
    std::shared_ptr<T> search(const double&);
    NodePointer insert(const double&, const T&);
    NodePointer insert(const double&, T&&);
    NodePointer& first(){return head->level[0].forward;}
    void del(const double&);
    unsigned long getRank();
    void print(std::ostream&);
    size_t len(){return length;}
};

template <typename T>
SkipList<T>::SkipList(const int& _max_level) {
    maxLevel = _max_level;
    head = std::make_shared<SkipListNode<T>>();
    head->level.resize(maxLevel);
    level = 1;
}

template <typename T>
int SkipList<T>::randomLevel() {
    std::uniform_int_distribution<int> distribution(0, 1);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);
    int randomlevel = 1;
    while (distribution(engine)) {
        randomlevel++;
    }
    return randomlevel > maxLevel ? maxLevel : randomlevel;
}

template <typename T>
std::shared_ptr<T> SkipList<T>::search(const double& score) {
    NodePointer node = head;
    if (node.get() == nullptr) {
        return nullptr;
    }
    for (int i = node->level.size() - 1; i >= 0; --i) {
        // next=node->level[i];
        while (node->level[i].forward.get() &&
               (node->level[i].forward->score <= score)) {
            if (node->level[i].forward->score == score) {
                // return &(node->level[i]->forward->data);
                return std::make_shared<T>(node->level[i].forward->data);
            }
            node = node->level[i].forward;
        }
    }
    return nullptr;
}

template <typename T>
std::shared_ptr<SkipListNode<T>> SkipList<T>::insert(const double& score,
                                                     const T& value) {
    NodePointer node = head;
    std::vector<NodePointer> update;
    update.resize(maxLevel);
    std::vector<unsigned int> rank;
    rank.resize(maxLevel);
    for (int i = level - 1; i >= 0; i--) {
        //从上往下查找，i从大到小，所以rank[i]=rank[i+1]
        rank[i] = i == (maxLevel - 1) ? 0 : rank[i + 1];
        /*
        while ((node->level[i].forward.get()) &&
               ((node->level[i].forward->score < socre) ||
                (node->level[i].forward->score == score) &&
                    (node->level[i].forward->data < value))) {
            // forward的score小于新socre或者forward的data小于新value，向前
            //此时rank[i]中已经存放了上一个节点的rank
            // node->level[i].span代表的是上一个节点到下一个节点的跨度
            rank[i] += node->level[i].span;
            node = node->level[i].forward;
        }
        */
        while ((node->level[i].forward.get()) &&
               (node->level[i].forward->score <= score)) {
            // forward的score小于新socre或者forward的data小于新value，向前
            //此时rank[i]中已经存放了上一个节点的rank
            // node->level[i].span代表的是上一个节点到下一个节点的跨度
            if (node->level[i].forward->score == score) {
                node->level[i].forward->data = value;
                return node->level[i].forward;
            }
            rank[i] += node->level[i].span;
            node = node->level[i].forward;
        }
        update[i] = node;
    }
    /*
    if (node->level[i].forward.get() &&
        node->level[i].forward->score == score &&
        node->level[i].forward->data == value) {
        return node->level[i].forward;
    }
    */
    // if()
    //随机产生level
    int rlevel = randomLevel();
    if (rlevel > this->level) {
        for (int i = this->level; i < rlevel; ++i) {
            rank[i] = 0;
            update[i] = head;
            //设置此处head的span为整个跳跃表的长度
            update[i]->level[i].span = length;
        }
        this->level = rlevel;
    }
    NodePointer newnode = std::make_shared<Node>();
    newnode->level.resize(rlevel);
    newnode->score = score;
    newnode->data = value;
    for (int i = 0; i < rlevel; i++) {
        newnode->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = newnode;
        //如果节点都在同一个节点中，那么他们的rank都是相同的
        // rank[0]代表的是level0的rank
        //此时update[0]代表要插入的位置
        //在update[0]后插入
        newnode->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        //更新前一个节点的span，span至少为1
        // update[i].forward指向新增节点，span为1
        // rank[0]减去rank[i]再加1，代表到新节点的跨度
        // rank[0]是新插入的节点前一个节点的rank
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;
    }
    //如果rlevel小于list中最大的level，则所有的update[i]
    //后继都不变，不过span需要+1
    for (int i = rlevel; i < level; ++i) {
        update[i]->level[i].span++;
    }
    //更新前驱指针
    //如果是除头结点外第一个节点，那么前驱为nullptr
    //否则更新为update[0]
    if (update[0].get()) {
        newnode->backward = update[0];
    }
    //如果存在后继
    //更新后继的前驱指针
    if (newnode->level[0].forward.get()) {
        newnode->level[0].forward->backward = newnode;
    } else {
        tail = newnode;
    }
    length++;
    return newnode;
}

template <typename T>
std::shared_ptr<SkipListNode<T>> SkipList<T>::insert(const double& score,
                                                     T&& value) {
    NodePointer node = head;
    std::vector<NodePointer> update;
    update.resize(maxLevel);
    std::vector<unsigned int> rank;
    rank.resize(maxLevel);
    for (int i = level - 1; i >= 0; i--) {
        //从上往下查找，i从大到小，所以rank[i]=rank[i+1]
        rank[i] = i == (maxLevel - 1) ? 0 : rank[i + 1];
        /*
        while ((node->level[i].forward.get()) &&
               ((node->level[i].forward->score < socre) ||
                (node->level[i].forward->score == score) &&
                    (node->level[i].forward->data < value))) {
            // forward的score小于新socre或者forward的data小于新value，向前
            //此时rank[i]中已经存放了上一个节点的rank
            // node->level[i].span代表的是上一个节点到下一个节点的跨度
            rank[i] += node->level[i].span;
            node = node->level[i].forward;
        }
        */
        while ((node->level[i].forward.get()) &&
               (node->level[i].forward->score <= score)) {
            // forward的score小于新socre或者forward的data小于新value，向前
            //此时rank[i]中已经存放了上一个节点的rank
            // node->level[i].span代表的是上一个节点到下一个节点的跨度
            if (node->level[i].forward->score == score) {
                node->level[i].forward->data = value;
                return node->level[i].forward;
            }
            rank[i] += node->level[i].span;
            node = node->level[i].forward;
        }
        update[i] = node;
    }
    /*
    if (node->level[i].forward.get() &&
        node->level[i].forward->score == score &&
        node->level[i].forward->data == value) {
        return node->level[i].forward;
    }
    */
    // if()
    //随机产生level
    int rlevel = randomLevel();
    if (rlevel > this->level) {
        for (int i = this->level; i < rlevel; ++i) {
            rank[i] = 0;
            update[i] = head;
            //设置此处head的span为整个跳跃表的长度
            update[i]->level[i].span = length;
        }
        this->level = rlevel;
    }
    NodePointer newnode = std::make_shared<Node>();
    newnode->level.resize(rlevel);
    newnode->score = score;
    newnode->data = std::move(value);
    for (int i = 0; i < rlevel; i++) {
        newnode->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = newnode;
        //如果节点都在同一个节点中，那么他们的rank都是相同的
        // rank[0]代表的是level0的rank
        //此时update[0]代表要插入的位置
        //在update[0]后插入
        newnode->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        //更新前一个节点的span，span至少为1
        // update[i].forward指向新增节点，span为1
        // rank[0]减去rank[i]再加1，代表到新节点的跨度
        // rank[0]是新插入的节点前一个节点的rank
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;
    }
    //如果rlevel小于list中最大的level，则所有的update[i]
    //后继都不变，不过span需要+1
    for (int i = rlevel; i < level; ++i) {
        update[i]->level[i].span++;
    }
    //更新前驱指针
    //如果是除头结点外第一个节点，那么前驱为nullptr
    //否则更新为update[0]
    if (update[0].get()) {
        newnode->backward = update[0];
    }
    //如果存在后继
    //更新后继的前驱指针
    if (newnode->level[0].forward.get()) {
        newnode->level[0].forward->backward = newnode;
    } else {
        tail = newnode;
    }
    length++;
    return newnode;
}

template <typename T>
void SkipList<T>::del(const double& score) {
    NodePointer node = head;
    std::vector<NodePointer> update;
    update.resize(this->level);
    for (int i = this->level - 1; i >= 0; i--) {
        while ((node->level[i].forward.get()) &&
               (node->level[i].forward->score < score)) {
            node = node->level[i].forward;
        }
        update[i] = node;
    }

    if (!node->level[0].forward.get() ||
        node->level[0].forward->score > score) {
        return;
    }
    NodePointer rmnode = node->level[0].forward;
    for (unsigned int i = 0; i < rmnode->level.size(); i++) {
        update[i]->level[i].forward = rmnode->level[i].forward;

        update[i]->level[i].span += rmnode->level[i].span - 1;
    }
    //如果rlevel小于list中最大的level，则所有的update[i]
    //后继都不变，不过span需要+1
    for (int i = rmnode->level.size(); i < level; ++i) {
        update[i]->level[i].span--;
    }
    //更新前驱指针
    //如果是除头结点外第一个节点，那么前驱为nullptr
    //否则更新为update[0]
    if (update[0].get()) {
        if (rmnode->level[0].forward.get()) {
            rmnode->level[0].forward->backward = update[0];
        } else {
            tail = update[0];
        }
    }
    length--;
}
template <typename T>
void SkipList<T>::print(std::ostream& out) {
    NodePointer node = head->level[0].forward;
    while (node.get()) {
        out << node->score << " " << node->data << '\n';
        node = node->level[0].forward;
    }
}
}  // namespace SDB

#endif