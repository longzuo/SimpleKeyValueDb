#ifndef SDB_LIST_HPP
#define SDB_LIST_HPP

#include <memory>

namespace SDB {
template <typename T>
class ListNode {
   public:
    T data;
    std::shared_ptr<ListNode> pre;
    std::shared_ptr<ListNode> next;
    bool hasNext(){return (next.get()!=nullptr);}
    bool hasPrevious(){return (pre.get()!=nullptr);}
};

template <typename T>
class List {
   public:
    using Node = ListNode<T>;
    using NodePointer = std::shared_ptr<Node>;

   private:
    NodePointer head;
    NodePointer tail;
    ssize_t length = 0;

   public:
    void push(const T&);
    NodePointer pop();
    ssize_t len() const { return length; }
    NodePointer first(){return head;}
    NodePointer last(){return last;}
};

template <typename T>
void List<T>::push(const T& _data) {
    if (head.get() == nullptr) {
        head = std::make_shared<Node>();
        tail = head;
    } else {
        tail->next = std::make_shared<Node>();
        tail = tail->next;
    }
    tail->data = _data;
    length++;
}

template <typename T>
typename List<T>::NodePointer List<T>::pop() {
    if(length==0)return nullptr;
    NodePointer res = head;
    head = head->next;
    length--;
    if (length == 0) {
        tail = head;
    }else
    {
        res->next.reset();
        head->pre.reset();
    }
    return res;
}

}  // namespace SDB
#endif