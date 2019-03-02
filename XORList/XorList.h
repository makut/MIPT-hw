# include <memory>
# include <iostream>
# include <iterator>

template<typename T, typename Allocator = std::allocator<T> >
class XorList
{
private:
    class Node;
public:
    class iterator : public std::iterator<std::bidirectional_iterator_tag, T>
    {
        friend XorList;
    public:
        explicit iterator(Node *prev = nullptr, Node *curr = nullptr):
            prev_(prev), curr_(curr) {}

        iterator(const iterator &other) = default;

        ~iterator() = default;

        T& operator*() const
        {
            return curr_->value_;
        }

        bool operator==(const iterator &other) const
        {
            return (curr_ == other.curr_);
        }

        bool operator!=(const iterator &other) const
        {
            return !operator==(other);
        }

        iterator& operator++()
        {
            prev_ = next_();
            std::swap(prev_, curr_);
            return *this;
        }

        iterator& operator--()
        {
            curr_ = getPtrNum(prev_->getXor() ^ getNumPtr(curr_));
            std::swap(prev_, curr_);
            return *this;
        }

        iterator operator++(int)
        {
            iterator cpy = *this;
            operator++();
            return cpy;
        }

        iterator operator--(int)
        {
            iterator cpy = *this;
            operator--();
            return cpy;
        }

        T* operator->()
        {
            return &(curr_->value_);
        }

        const T* operator->() const
        {
            return &(curr_->value_);
        }
    private:
        Node *prev_, *curr_;

        Node* next_() const
        {
            return getPtrNum(getNumPtr(prev_) ^ curr_->getXor());
        }
    };
    typedef std::reverse_iterator<iterator> reverse_iterator;

    explicit XorList(const Allocator &alloc = Allocator()):
        alloc_(alloc), first_(nullptr), last_(nullptr), size_(0) {}

    XorList(size_t count, const T &value = T(), const Allocator &alloc = Allocator()) 
    : XorList(alloc)
    {
        for (size_t i = 0; i < count; i++)
            push_back(value);
    }

    XorList(const XorList &other) : XorList(other.alloc_)
    {
        for (iterator it = other.begin(); it != other.end(); ++it)
            push_back(*it);
    }

    XorList(XorList&& other) noexcept 
    : alloc_(other.alloc_), first_(nullptr), last_(nullptr), size_(other.size_)
    {
        other.size_ = 0;
        std::swap(first_, other.first_);
        std::swap(last_, other.last_);
    }

    XorList(std::initializer_list<T> init_lst, const Allocator &alloc = Allocator())
    : XorList(alloc)
    {
        for (auto it = init_lst.begin(); it != init_lst.end(); ++it)
            push_back(*it);
    }

    ~XorList()
    {
        while (!empty())
            pop_front();
    }

    XorList& operator=(const XorList &other)
    {
        for (iterator it = other.begin(); it != other.end(); ++it)
        {
            push_back(*it);
        }
        return *this;
    }

    XorList& operator=(XorList&& other) noexcept
    {
        size_ = other.size_;
        other.size_ = 0;
        alloc_ = other.alloc_;
        std::swap(first_, other.first_);
        std::swap(last_, other.last_);
        other.first_ = nullptr;
        other.last_ = nullptr;
    }

    size_t size() const
    {
        return size_;
    }

    bool empty() const
    {
        return size() == 0;
    }

    template<typename U>
    void push_back(U&& value)
    {
        insert_before(end(), std::forward<U>(value));
    }

    template<typename U>
    void push_front(U&& value)
    {
        insert_before(begin(), std::forward<U>(value));
    }

    void pop_back()
    {
        erase(--end());
    }

    void pop_front()
    {
        erase(begin());
    }

    template<typename U>
    void insert_before(const iterator &it, U&& value)
    {
        ++size_;
        Node *new_node = new(std::allocator_traits<NodeAllocType>::allocate(alloc_, 1)) Node(std::forward<U>(value));
        Node *left = it.prev_, *right = it.curr_;
        Node::makeLink(left, right);
        Node::makeLink(left, new_node);
        Node::makeLink(new_node, right);
        if (it.prev_ == nullptr)
            first_ = new_node;
        if (it.curr_ == nullptr)
            last_ = new_node;
    }

    void erase(iterator it)
    {
        --size_;
        if (it.prev_ == nullptr)
            first_ = it.next_();
        if (it.next_() == nullptr)
            last_ = it.prev_;
        Node *curr = it.curr_, *left = it.prev_, *right = it.next_();
        Node::makeLink(left, curr);
        Node::makeLink(curr, right);
        Node::makeLink(left, right);
        curr->~Node();
        std::allocator_traits<NodeAllocType>::deallocate(alloc_, curr, 1);

    }

    void insert_after(iterator it, const T &value)
    {
        insert_before(++it, value);
    }

    iterator begin() const
    {
        return iterator(nullptr, first_);
    }

    iterator end() const
    {
        return iterator(last_, nullptr);
    }

    reverse_iterator rbegin() const
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend() const
    {
        return reverse_iterator(begin());
    }
private:
    class Node
    {
        friend class iterator;
    public:
        Node(const T &new_value, uintptr_t new_xor = 0): 
            value_(new_value), xor_(new_xor) {}

        Node(T&& new_value, uintptr_t new_xor = 0):
            value_(std::move(new_value)), xor_(new_xor) {}

        Node& operator=(const Node &other) = delete;

        Node(const Node &other) = delete;

        Node(Node&& other) = delete;

        ~Node() = default;

        uintptr_t getXor() const
        {
            return xor_;
        }

        void updateXor(Node *new_ptr, Node *deleted_ptr = nullptr)
        {
            xor_ ^= (getNumPtr(new_ptr) ^ getNumPtr(deleted_ptr));
        }

        static void makeLink(Node *first, Node *second)
        {
            if (first == nullptr || second == nullptr)
                return;
            first->updateXor(second);
            second->updateXor(first);
        }
    private:
        T value_;
        uintptr_t xor_;
    };

    typedef typename Allocator::template rebind<Node>::other NodeAllocType;
    NodeAllocType alloc_;
    Node *first_, *last_;
    size_t size_;

    static uintptr_t getNumPtr(Node *ptr)
    {
        if (ptr == nullptr)
            return 0;
        return reinterpret_cast<uintptr_t>(ptr);
    }

    static Node* getPtrNum(uintptr_t addr)
    {
        return reinterpret_cast<Node*>(addr);
    }
};