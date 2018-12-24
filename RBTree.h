#include "List.h"
#include <cassert>
#include <memory>

// 1. No red node has a red child.
// 2. Every path from root to empty node contains the same
// number of black nodes.

template<class T>
class RBTree
{
    enum Color { R, B };

    struct Node
    {
        Node(Color c, 
            std::shared_ptr<const Node> const & lft, 
            T val, 
            std::shared_ptr<const Node> const & rgt,
	    bool act)
            : _c(c), _lft(lft), _val(val), _rgt(rgt),active(act)
        {}
        Color _c;	
        std::shared_ptr<const Node> _lft;
        T _val;
        std::shared_ptr<const Node> _rgt;
        bool active;
    };
    explicit RBTree(std::shared_ptr<const Node> const & node) : _root(node) {} 
    Color rootColor() const
    {
        assert (!isEmpty());
        return _root->_c;
    }

public:
    RBTree() {}
    RBTree(Color c, RBTree const & lft, T val, RBTree const & rgt,bool act)
        : _root(std::make_shared<const Node>(c, lft._root, val, rgt._root,act))
    {
        assert(lft.isEmpty() || lft.root() < val);
        assert(rgt.isEmpty() || val < rgt.root());
    }
    RBTree(std::initializer_list<T> init)
    {
        RBTree t;
        for (T v : init)
        {
            t = t.inserted(v);
        }
        _root = t._root;
    }
    template<class I>
    RBTree(I b, I e)
    {
        RBTree t;
        for_each(b, e, [&t](T const & v){
            t = t.inserted(v);
        });
        _root = t._root;
    }
    bool isEmpty() const { return !_root; }
    T root() const
    {
        assert(!isEmpty());
        return _root->_val;
    }
    RBTree getroot() const
    { 
	assert(!isEmpty());
        return RBTree(_root);
    }
    RBTree left() const
    {
        assert(!isEmpty());
        return RBTree(_root->_lft);
    }
    RBTree right() const
    {
        assert(!isEmpty());
        return RBTree(_root->_rgt);
    }
    bool getactive() const
    {
        assert(!isEmpty());
        return _root->active;
    }
    bool member(T x) const
    {
        if (isEmpty())
            return false;
        T y = root();
        if (x < y)
            return left().member(x);
        else if (y < x)
            return right().member(x);
        else
	{
	    if (getactive() == false)
	      return false;	
            return true;
        }
    } 
    /*bool setActiveFalse(T x)
    {
        T y = root();
        if (x < y)
            return left().setActiveFalse(x);
        else if (y < x)
            return right().setActiveFalse(x);
        else
	{
            setactive(false);
	    return false;
	}
    }*/
    bool ismember(T x) const
	{
	  T y = root();
	  if(x == y)
		return true;
	  else 
		return false;
	}
    RBTree inserted(T x) 
    {
        RBTree t = ins(x);
        return RBTree(B, t.left(), t.root(), t.right(),t.getactive());
    }
    RBTree deleted(T x) 
    {
        RBTree t = del(x);
        return RBTree(B, t.left(), t.root(), t.right(),t.getactive());
    }
    RBTree ins(T x) 
    {
        assert1();
        if (isEmpty())
            return RBTree(R, RBTree(), x, RBTree(), true);
        T y = root();
        Color c = rootColor();
        if (rootColor() == B)
        {
            if (x < y)
                return balance(left().ins(x), y, right(),getactive());
            else if (y < x)
                return balance(left(), y, right().ins(x),getactive());
            else
	    {
                return *this; // no duplicates
	    }
        }
        else
        {
            if (x < y)
                return RBTree(c, left().ins(x), y, right(),getactive());
            else if (y < x)
                return RBTree(c, left(), y, right().ins(x),getactive());
            else
            {
                return *this; // no duplicates
	    }
        }
    }
    RBTree del(T x)
    {
        assert1();
        if (isEmpty())
            return RBTree(R, RBTree(), x, RBTree(), false);
        T y = root();
        Color c = rootColor();
        if (rootColor() == B)
        {
            if (x < y)
                return balance(left().del(x), y, right(),getactive());
            else if (y < x)
                return balance(left(), y, right().del(x),getactive());
            else
	    {
		RBTree t = RBTree(c,left(),y,right(),false);
                return t; // no duplicates
	    }
        }
        else
        {
            if (x < y)
                return RBTree(c, left().del(x), y, right(),getactive());
            else if (y < x)
                return RBTree(c, left(), y, right().del(x),getactive());
            else
            {
  		RBTree t = RBTree(c,left(),y,right(),false);
                return t; // no duplicates
	    }
        }
    }
    // 1. No red node has a red child.
    void assert1() const
    {
        if (!isEmpty())
        {
            auto lft = left();
            auto rgt = right();
            if (rootColor() == R) 
            {
                assert(lft.isEmpty() || lft.rootColor() == B);
                assert(rgt.isEmpty() || rgt.rootColor() == B);
            }
            lft.assert1();
            rgt.assert1();
        }
    }
    // 2. Every path from root to empty node contains the same
    // number of black nodes.
    int countB() const
    {
        if (isEmpty())
            return 0;
        int lft = left().countB();
        int rgt = right().countB();
        assert(lft == rgt);
        return (rootColor() == B)? 1 + lft: lft;
    }
    static RBTree balance(RBTree const & lft, T x, RBTree const & rgt,bool act)
    {
        if (lft.doubledLeft())
            return RBTree(R
                        , lft.left().paint(B)
                        , lft.root()
                        , RBTree(B, lft.right(), x, rgt,act),lft.getactive());
        else if (lft.doubledRight())
            return RBTree(R
                        , RBTree(B, lft.left(), lft.root(), lft.right().left(),lft.getactive())
                        , lft.right().root()
                        , RBTree(B, lft.right().right(), x, rgt,act),lft.right().getactive());
        else if (rgt.doubledLeft())
            return RBTree(R
                        , RBTree(B, lft, x, rgt.left().left(),act)
                        , rgt.left().root()
                        , RBTree(B, rgt.left().right(), rgt.root(), rgt.right(),rgt.getactive()),rgt.left().getactive());
        else if (rgt.doubledRight())
            return RBTree(R
                        , RBTree(B, lft, x, rgt.left(),act)
                        , rgt.root()
                        , rgt.right().paint(B),rgt.getactive());
        else
            return RBTree(B, lft, x, rgt,act);
    }
private:
 
    
	
    // Called only when parent is black
    
    bool doubledLeft() const 
    {
        return !isEmpty()
            && rootColor() == R
            && !left().isEmpty()
            && left().rootColor() == R;
    }
    bool doubledRight() const 
    {
        return !isEmpty()
            && rootColor() == R
            && !right().isEmpty()
            && right().rootColor() == R;
    }
    RBTree paint(Color c) const
    {
        assert(!isEmpty());
        return RBTree(c, left(), root(), right(),getactive());
    }
private:
    std::shared_ptr<const Node> _root;
};








template<class T, class F>
void forEach(RBTree<T> const & t, F f) {
    if (!t.isEmpty()) {
        forEach(t.left(), f);
        f(t.root());
        forEach(t.right(), f);
    }
}

template<class T, class Beg, class End>
RBTree<T> inserted(RBTree<T> t, Beg it, End end)
{
    if (it == end)
        return t;
    T item = *it;
    auto t1 = inserted(t, ++it, end);
    return t1.inserted(item);
}

template<class T>
RBTree<T> treeUnion(RBTree<T> const & a, RBTree<T> const & b)
{
    // a u b = a + (b \ a)
    RBTree<T> res = a;
    forEach(b, [&res, &a](T const & v){
        if (!a.member(v))
            res.inserted(v);
    });
    return res;
}

template<class T>
RBTree<T> deleted(T const & a, RBTree<T> & t)
{
    t = t.deleted(a);
    return t;
}
// Remove elements in set from a list
template<class T>
List<T> rem_from_list(List<T> const & lst, RBTree<T> const & set)
{
    List<T> res;
    lst.forEach([&res, &set](T const & v) {
        if (!set.member(v))
            res = res.pushed_front(v);
    });
    return res;
}

