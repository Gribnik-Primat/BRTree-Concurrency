#if ! defined(LIST_H)
#define LIST_H

#include <cassert>
#include <memory>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <iostream> // print

template<class T>
class List
{
    struct Item
    {
        Item(T v, std::shared_ptr<const Item> tail)
            : _val(v), _next(tail)
        {}
        // singleton
        explicit Item(T v) : _val(v) {}
        //~Item() { std::cout << "~" << _val << std::endl; }
        T _val;
        std::shared_ptr<const Item> _next;
    };
    friend Item;
    explicit List(std::shared_ptr<const Item> items)
        : _head(items) {}
public:
    // Empty list
    List() {}
   // Cons
    List(T v, List const & tail)
        : _head(std::make_shared<Item>(v, tail._head))
    {
    }
    // Singleton
    explicit List(T v) : _head(std::make_shared<Item>(v)) {}
    // From initializer list
    List(std::initializer_list<T> init)
    {
        for (auto it = std::rbegin(init); it != std::rend(init); ++it)
        {
            _head = std::make_shared<Item>(*it, _head);
        }
    }

    bool isEmpty() const { return !_head; } // conversion to bool
    T front() const
    {
        assert(!isEmpty());
        return _head->_val;
    }
    List popped_front() const
    {
        assert(!isEmpty());
        return List(_head->_next);
    }
private:
    std::shared_ptr<const Item> _head;
};


// Pass lst by value not reference!
template<class T>
void printRaw(List<T> lst)
{
    if (lst.isEmpty()) {
        std::cout << std::endl;
    }
    else {
        std::cout << lst.front() << " ";
        printRaw(lst.popped_front());
    }
}

#endif
