// https://github.com/makut/homework

# include <iterator>
# include <cstring>
# include <algorithm>

template<typename T>
class Stack
{
private:
    size_t capacity_, head_;
    T *elements_, *temporal_;
    static const size_t DEFAULT_SIZE_;

    void changeCapacity(const int &new_capacity)
    {
       temporal_ = new T[new_capacity];
       std::copy(elements_, elements_ + std::max(DEFAULT_SIZE_, head_), temporal_);
       delete[] elements_;
       elements_ = temporal_;
       capacity_ = new_capacity;
    }
public:
    explicit Stack(const size_t size = DEFAULT_SIZE_) : capacity_(size), head_(0)
    {
        elements_ = new T[size];
    }

    Stack(const size_t size, const T *new_elements_) : capacity_(std::max(size, DEFAULT_SIZE_)), head_(size)
    {
        elements_ = new T[std::max(size, DEFAULT_SIZE_)];
        std::copy(new_elements_, new_elements_ + size, elements_);
    }

    Stack& operator=(const Stack &other)
    {
        if (this == &other) return *this;
        capacity_ = other.capacity_;
        head_ = other.head_;
        elements_ = new T[capacity_];
        memcpy(elements_, other.elements_, capacity_ * sizeof(T));
        std::copy(other.elements_, other.elements_ + capacity_, elements_);
        return *this;
    }

    Stack(const Stack &other)
    {
        *this = other;
    }

    ~Stack()
    {
        delete[] elements_;
    }

    size_t size() const
    {
        return head_;
    }

    void push(const T &added)
    {
        if (head_ == capacity_)
        {
            changeCapacity(capacity_ * 2);
        }
        elements_[head_++] = added;
    }

    T pop()
    {
        T result = elements_[--head_];
        if (head_ <= capacity_ / 4 && capacity_ > DEFAULT_SIZE_)
        {
            changeCapacity(capacity_ / 2);
        }
        return result;
    }

    T& top()
    {
        return elements_[head_ - 1];
    }

    const T& top() const
    {
        return elements_[head_ - 1];
    }

    T& operator[](const size_t &index)
    {
        return elements_[index];
    }

    const T& operator[](const size_t &index) const
    {
        return elements_[index];
    }

    bool empty() const
    {
        return (head_ == 0);
    }
};
template<typename T>
const size_t Stack<T>::DEFAULT_SIZE_ = 10;

template<typename T>
void shift_(Stack<T> &from, Stack<T> &to)
{
    size_t capacity_ = from.size();
    if (capacity_ == 1)
    {
        to.push(from.pop());
        return;
    }
    Stack<T> temporal_;
    for (size_t i = 0; i < capacity_ / 2; i++)
    {
        temporal_.push(from.pop());
    }
    while (!from.empty())
    {
        to.push(from.pop());
    }
    while (!temporal_.empty())
    {
        from.push(temporal_.pop());
    }
}


template<typename T>
class Deque
{
private:
    Stack<T> left_, right_;

    template<typename Type, typename Ptr>
    class DequeIterator_ : public std::iterator<std::random_access_iterator_tag, Type>
    {
    private:
        size_t index_;
        Ptr seq;
    public:
        typedef typename std::iterator_traits<DequeIterator_>::difference_type diff_type;
        DequeIterator_(Ptr sequence = NULL, size_t ind = 0) : index_(ind), seq(sequence) {}
        DequeIterator_(const DequeIterator_ &other) : index_(other.index_), seq(other.seq) {}

        DequeIterator_& operator=(const DequeIterator_ &other)
        {
            index_ = other.index_;
            seq = other.seq;
            return *this;
        }

        Type& operator*() const
        {
            return (*seq)[index_];
        }

        bool operator==(const DequeIterator_ &other) const
        {
            return (seq == other.seq && index_ == other.index_);
        }

        bool operator!=(const DequeIterator_ &other) const
        {
            return !operator==(other);
        }

        diff_type operator-(const DequeIterator_ &other) const
        {
            return (index_ - other.index_);
        }

        bool operator<(const DequeIterator_ &other) const
        {
            return (operator-(other) < 0);
        }

        bool operator>(const DequeIterator_ &other) const
        {
            return (other < (*this));
        }
        bool operator<=(const DequeIterator_ &other) const
        {
            return (operator<(other) || operator==(other));
        }

        bool operator>=(const DequeIterator_ &other) const
        {
            return (operator>(other) || operator==(other));
        }

        DequeIterator_& operator++()
        {
            ++index_;
            return *this;
        }

        DequeIterator_ operator++(int)
        {
            DequeIterator_ cpy = *this;
            ++index_;
            return cpy;
        }

        DequeIterator_& operator--()
        {
            --index_;
            return *this;
        }

        DequeIterator_ operator--(int)
        {
            DequeIterator_ cpy = *this;
            --index_;
            return cpy;
        }

        DequeIterator_ operator+(const diff_type &n) const
        {
            DequeIterator_ ans(seq, index_ + n);
            return ans;
        }

        friend DequeIterator_ operator+(const diff_type &n, const DequeIterator_ &it)
        {
            DequeIterator_ ans(it.seq, it.index_ + n);
            return ans;
        }

        DequeIterator_ operator-(const diff_type &n) const
        {
            return operator+(-n);
        }
        friend DequeIterator_ operator-(const diff_type &n, const DequeIterator_ &it)
        {
            return it + (-n);
        }

        void operator+=(const diff_type &n)
        {
            index_ += n;
        }

        void operator-=(const diff_type &n)
        {
            index_ -= n;
        }

        Type* operator->()
        {
            return &((*seq)[index_]);
        }

        const Type* operator->() const
        {
            return &((*seq)[index_]);
        }
    };
public:
    typedef DequeIterator_<T, Deque<T>*> iterator;
    typedef DequeIterator_<const T, const Deque<T>*> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    Deque(){}

    Deque& operator=(const Deque &other)
    {
        if (this == &other) return *this;
        left_ = other.left_;
        right_ = other.right_;
        return *this;
    }

    Deque(const Deque &other)
    {
        *this = other;
    }

    void push_back(const T &x)
    {
        left_.push(x);
    }

    void push_front(const T &x)
    {
        right_.push(x);
    }

    T pop_back()
    {
        if (left_.empty())
        {
            shift_(right_, left_);
        }
        return left_.pop();
    }

    T pop_front()
    {
        if (right_.empty())
        {
            shift_(left_, right_);
        }
        return right_.pop();
    }

    bool empty() const
    {
        return (left_.empty() && right_.empty());
    }

    size_t size() const
    {
        return (left_.size() + right_.size());
    }

    T& back()
    {
        if (!left_.empty()) return left_.top();
        else return right_[0];
    }

    const T& back() const
    {
        if (!left_.empty()) return left_.top();
        else return right_[0];
    }

    T& front()
    {
        if (!right_.empty()) return right_.top();
        else return left_[0];
    }

    const T& front() const
    {
        if (!right_.empty()) return right_.top();
        else return left_[0];
    }

    T& operator[](const size_t &index)
    {
        if (index < right_.size()) return right_[right_.size() - index - 1];
        else return left_[index - right_.size()];
    }

    const T& operator[](const size_t &index) const
    {
        if (index < right_.size()) return right_[right_.size() - index - 1];
        else return left_[index - right_.size()];
    }

    iterator begin()
    {
        return iterator(this, 0);
    }

    const_iterator begin() const
    {
        return const_iterator(this, 0);
    }

    const_iterator cbegin() const
    {
        return const_iterator(this, 0);
    }

    iterator end()
    {
        return iterator(this, size());
    }

    const_iterator end() const
    {
        return const_iterator(this, size());
    }

    const_iterator cend() const
    {
        return const_iterator(this, size());
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(cbegin());
    }
};
