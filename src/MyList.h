#include <iostream>

template <typename T, typename Alloc>
class MyList
{
    struct Node
    {
        Node* next;
        T val;
        Node(T v) : next(nullptr), val(v){ };
    };
public:
    bool empty() { return head == nullptr;}; 
    void push_back(const T& val)
    {
        auto newNode = nodeAlloc.allocate(1);
        nodeAlloc.construct(newNode, val);
        if(empty())
        {
            back = head = newNode;
        }
        else
        {
            back->next = newNode;
            back = newNode;
        }
    }
    void remove_head()
    {
        if(empty()) return;
        auto p = head;
        head = p->next;
        nodeAlloc.deallocate(p, 1);
    }
    void remove_back()
    {
        if(empty()) return;
        if(head == back) remove_head();
        else
        {
            auto p = head;
            while (p->next != back) p = p->next;
            p->next = nullptr;
            delete back;
            back = p;
        }
    }

    void print() 
    {
        if (empty()) return;
        auto p = head;
        while (p != back) 
        {
            std::cout << p->val << "->";
            p = p->next;
        }
        std::cout << p->val << std::endl;
    }

private:
    Node* head = nullptr;
    Node* back = nullptr;

    typename Alloc::template rebind<Node>::other nodeAlloc;
};