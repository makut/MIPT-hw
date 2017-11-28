// https://github.com/makut/homework

# include <iterator>
# include <cstring>

template<typename T>
class Stack
{
private:
    size_t capacity, head;
    T *elements, *temporal;
    static const size_t default_size;
public:
    Stack(const size_t size = default_size) : capacity(size), head(0)
    {
        elements = new T[size];
    }

    Stack(const size_t size, const T *new_elements) : capacity(std::max(size, default_size)), head(size)
    {
        elements = new T[std::max(size, default_size)];
        memcpy(elements, new_elements, size * sizeof(T));
    }

    Stack& operator=(const Stack &other)
    {
        if (this == &other) return *this;
        capacity = other.capacity;
        head = other.head;
        elements = new T[capacity];
        memcpy(elements, other.elements, capacity * sizeof(T));
        return *this;
    }

    Stack(const Stack &other)
    {
        *this = other;
    }

    ~Stack()
    {
        delete[] elements;
    }

    size_t size() const
    {
        return head;
    }

    void push(const T &added)
    {
        if (head == capacity)
        {
            temporal = new T[capacity * 2];
            memcpy(temporal, elements, capacity * sizeof(T));
            delete[] elements;
            elements = temporal;
            capacity *= 2;
        }
        elements[head++] = added;
    }

    T pop()
    {
        T result = elements[--head];
        if (head <= capacity / 4 && capacity > default_size)
        {
            temporal = new T[capacity / 2];
            memcpy(temporal, elements, capacity / 4 * sizeof(T));
            delete[] elements;
            elements = temporal;
            capacity /= 2;
        }
        return result;
    }

    T& top()
    {
        return elements[head - 1];
    }

    const T& top() const
    {
        return elements[head - 1];
    }

    T& operator[](const size_t &index)
    {
        return elements[index];
    }

    const T& operator[](const size_t &index) const
    {
        return elements[index];
    }

    bool empty() const
    {
        return (head == 0);
    }
};
template<typename T>
const size_t Stack<T>::default_size = 10;

template<typename T>
class Deque
{
private:
    Stack<T> left, right;

    template<typename Type, typename Ptr>
    class DequeIterator_ : public std::iterator<std::random_access_iterator_tag, Type>
    {
    private:
        size_t index;
        Ptr seq;
    public:
        typedef typename std::iterator_traits<DequeIterator_>::difference_type diff_type;
        DequeIterator_(Ptr sequence = NULL, size_t ind = 0) : index(ind), seq(sequence) {}
        DequeIterator_(const DequeIterator_ &other) : index(other.index), seq(other.seq) {}

        DequeIterator_& operator=(const DequeIterator_ &other)
        {
            index = other.index;
            seq = other.seq;
            return *this;
        }

        Type& operator*() const
        {
            return (*seq)[index];
        }

        bool operator==(const DequeIterator_ &other) const
        {
            return (seq == other.seq && index == other.index);
        }

        bool operator!=(const DequeIterator_ &other) const
        {
            return !operator==(other);
        }

        diff_type operator-(const DequeIterator_ &other) const
        {
            return (index - other.index);
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
            ++index;
            return *this;
        }

        DequeIterator_ operator++(int)
        {
            DequeIterator_ cpy = *this;
            ++index;
            return cpy;
        }

        DequeIterator_& operator--()
        {
            --index;
            return *this;
        }

        DequeIterator_ operator--(int)
        {
            DequeIterator_ cpy = *this;
            --index;
            return cpy;
        }

        DequeIterator_ operator+(const diff_type &n) const
        {
            DequeIterator_ ans(seq, index + n);
            return ans;
        }

        friend DequeIterator_ operator+(const diff_type &n, const DequeIterator_ &it)
        {
            DequeIterator_ ans(it.seq, it.index + n);
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
            index += n;
        }

        void operator-=(const diff_type &n)
        {
            index -= n;
        }

        Type* operator->()
        {
            return &((*seq)[index]);
        }

        const Type* operator->() const
        {
            return &((*seq)[index]);
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
        left = other.left;
        right = other.right;
        return *this;
    }

    Deque(const Deque &other)
    {
        *this = other;
    }

    void push_back(const T &x)
    {
        left.push(x);
    }

    void push_front(const T &x)
    {
        right.push(x);
    }

    T pop_back()
    {
        if (left.empty())
        {
            size_t capacity = right.size();
            if (capacity == 1)
            {
                return right.pop();
            }
            Stack<T> temporal;
            for (size_t i = 0; i < capacity / 2; i++)
            {
                temporal.push(right.pop());
            }
            while (!right.empty())
            {
                left.push(right.pop());
            }
            while (!temporal.empty())
            {
                right.push(temporal.pop());
            }
        }
        return left.pop();
    }

    T pop_front()
    {
        if (right.empty())
        {
            size_t capacity = left.size();
            if (capacity == 1) return left.pop();
            Stack<T> temporal;
            for (size_t i = 0; i < capacity / 2; i++)
            {
                temporal.push(left.pop());
            }
            while (!left.empty())
            {
                right.push(left.pop());
            }
            while (!temporal.empty())
            {
                left.push(temporal.pop());
            }
        }
        return right.pop();
    }

    bool empty() const
    {
        return (left.empty() && right.empty());
    }

    size_t size() const
    {
        return (left.size() + right.size());
    }

    T& back()
    {
        if (!left.empty()) return left.top();
        else return right[0];
    }

    const T& back() const
    {
        if (!left.empty()) return left.top();
        else return right[0];
    }

    T& front()
    {
        if (!right.empty()) return right.top();
        else return left[0];
    }

    const T& front() const
    {
        if (!right.empty()) return right.top();
        else return left[0];
    }

    T& operator[](const size_t &index)
    {
        if (index < right.size()) return right[right.size() - index - 1];
        else return left[index - right.size()];
    }

    const T& operator[](const size_t &index) const
    {
        if (index < right.size()) return right[right.size() - index - 1];
        else return left[index - right.size()];
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
