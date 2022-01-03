/* 基于智能指针实现双向链表 */
#include <iostream>
#include <cstdio>
#include <memory>
#include <iterator>

template <typename T>
struct Node {
    // 这两个指针会造成什么问题？请修复
    std::unique_ptr<Node<T>> next;
    Node *prev;
    // 如果能改成 unique_ptr 就更好了!
    // --> 修改next指针为unique_ptr, prev指针为Node<T> *

    T value;

    // 这个构造函数有什么可以改进的？ --> 使用初始化列表避免赋值
    Node(T val): value(val), prev(nullptr) {}

    // 当前节点向后插入
    void insert(const T &val) {
        auto node = std::make_unique<Node<T>>(val);
        // 弱引用
        if (next) {
            node->prev = next->prev;
            next->prev = node.get();
        } else {
            node->prev = this;
        }
        // 引用
        node->next = std::move(next);
        next = std::move(node); 
    }

    //  删除当前节点
    void erase() {
        if (next)
            next->prev = prev;
        if (prev)
            prev->next = std::move(next);
    }
  
    ~Node() {
        printf("~Node()\n");   // 应输出多少次？为什么少了？ --> 只执行了浅复制
    }
};

template <typename T>
struct _list_iterator: public std::iterator<std::bidirectional_iterator_tag, Node<T>>
{
private:
    Node<T> *_pn;

public:
    _list_iterator(Node<T> *pNode): _pn(pNode) {};
    _list_iterator(const _list_iterator &iter) = default;
    _list_iterator &operator=(const _list_iterator &iter) = default;

    _list_iterator &operator++()
    {
        _pn = _pn->next.get();
        return *this;
    }

    _list_iterator operator++(int)
    {
        _list_iterator temp(*this);
        _pn = _pn->next.get();
        return temp;
    }

    _list_iterator &operator--()
    {
        _pn = _pn->prev;
        return *this;
    }

    _list_iterator operator--(int)
    {
        _list_iterator temp(*this);
        _pn = _pn->prev;
        return temp;
    }

    bool operator==(const _list_iterator& rhs) const {return _pn==rhs._pn;}
    bool operator!=(const _list_iterator& rhs) const {return _pn!=rhs._pn;}

    T operator*() {return _pn->value; }
};

template <typename T>
struct List {
    std::unique_ptr<Node<T>> head;
    Node<T> *tail;

    using value_type = T;
    using iterator = _list_iterator<T>;
    iterator begin() const { return iterator(head.get()); }
    iterator end() const { return iterator(nullptr); }
    iterator back() const { return iterator(tail); }

    List() = default;

    List(List const &other) {
        printf("List 被拷贝！\n");
        // head = other.head;  // 这是浅拷贝！
        // 请实现拷贝构造函数为 **深拷贝**

        // 新加深拷贝
        std::back_insert_iterator<List> back_iter(*this);
        std::copy(other.begin(), other.end(), back_iter);
    }

    List &operator=(List const &) = delete;  // 为什么删除拷贝赋值函数也不出错？ --> 可以认为在原地创建一个匿名对象，然后调用移动赋值

    List(List &&) = default;
    List &operator=(List &&) = default;

    Node<T> *front() const {
        return head.get();
    }

    T pop_front() {
        T ret = head->value;
        head = std::move(head->next);
        head->prev = nullptr;
        return ret;
    }

    void push_front(const T &value) {
        auto node = std::make_unique<Node<T>>(value);
        if (head)
            head->prev = node.get();
        node->next = std::move(head);
        head = std::move(node);
        if(!tail)
            tail = head.get();
    }

    void push_back(const T &value) {
        auto node = std::make_unique<Node<T>>(value);
        if(head) {
            tail->insert(value);
            tail = tail->next.get();
        }
        else {
            head = std::move(node);
            tail = head.get();
        }
    }

    Node<T> *at(size_t index) const {
        auto curr = front();
        for (size_t i = 0; i < index; i++) {
            curr = curr->next.get();
        }
        return curr;
    }
};

void print(const List<int> &lst) {  // 有什么值得改进的？ --> 使用常用引用避免不必要的拷贝
    printf("[");
    for (auto item: lst) {
        printf(" %d", item);
    }
    printf(" ]\n");
}

int main() {
    List<int> a;

    a.push_front(7);
    a.push_front(5);
    a.push_front(8);
    a.push_front(2);
    a.push_front(9);
    a.push_front(4);
    a.push_front(1);

    print(a);   // [ 1 4 9 2 8 5 7 ]

    a.at(2)->erase();

    print(a);   // [ 1 4 2 8 5 7 ]

    List b = a;

    a.at(3)->erase();

    print(a);   // [ 1 4 2 5 7 ]
    print(b);   // [ 1 4 2 8 5 7 ]

    b = {};
    a = {};

    return 0;
}
