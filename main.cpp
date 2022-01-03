/* 基于智能指针实现双向链表 */
#include <cstdio>
#include <memory>

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

    // 当前节点向后插入，方便复制List
    void insert(const T &val) {
        auto node = std::make_unique<Node<T>>(val);
        // 弱引用
        node->prev = next->prev;
        next->prev = node;
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
        printf("~Node()\n");   // 应输出多少次？为什么少了？
    }
};

template <typename T>
struct List {
    std::unique_ptr<Node<T>> head;

    List() = default;

    List(List const &other) {
        printf("List 被拷贝！\n");
        head = other.head;  // 这是浅拷贝！
        // 请实现拷贝构造函数为 **深拷贝**
        Node *curs = head
        while (curs !== nullptr)
        {
            curs->insert()
        }
        
        
    }

    List &operator=(List const &) = delete;  // 为什么删除拷贝赋值函数也不出错？

    List(List &&) = default;
    List &operator=(List &&) = default;

    Node *front() const {
        return head.get();
    }

    int pop_front() {
        int ret = head->value;
        head = head->next;
        return ret;
    }

    void push_front(int value) {
        auto node = std::make_shared<Node>(value);
        node->next = head;
        if (head)
            head->prev = node;
        head = node;
    }

    Node *at(size_t index) const {
        auto curr = front();
        for (size_t i = 0; i < index; i++) {
            curr = curr->next.get();
        }
        return curr;
    }
};

void print(const List &lst) {  // 有什么值得改进的？ --> 使用常用引用避免不必要的拷贝
    printf("[");
    for (auto curr = lst.front(); curr; curr = curr->next.get()) {
        printf(" %d", curr->value);
    }
    printf(" ]\n");
}

int main() {
    List a;

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
