# include <memory>

class Block
{
public:

    explicit Block(Block *previous = nullptr, size_t size = SIZE):
        begin_(new char[size]), end_(begin_ + size), 
        curr_(reinterpret_cast<void*>(begin_)),
        space_(size), prev_(previous) {}

    ~Block()
    {
        delete[] begin_;
        if (prev_ != nullptr)
            delete prev_;
    }

    template<typename T>
    T* allocAligned(size_t n, size_t alignment = alignof(T))
    {
        if (std::align(alignment, n * sizeof(T), curr_, space_))
        {
            T *result = reinterpret_cast<T*>(curr_);
            curr_ = reinterpret_cast<char*>(curr_) + n * sizeof(T);
            space_ -= sizeof(T);
            return result;
        }
        return nullptr;
    }
private:
    char *begin_, *end_;
    void *curr_;
    size_t space_;
    Block *prev_;
    static const size_t SIZE;
};
const size_t Block::SIZE = 100000;

class MemoryBlocks
{
public:
    MemoryBlocks() : cnt_(0), curr_(nullptr) {}

    ~MemoryBlocks()
    {
        if (curr_ != nullptr)
            delete curr_;
    }

    void addReference()
    {
        ++cnt_;
    }

    void eraseReference()
    {
        --cnt_;
    }

    size_t getReferencesNum() const
    {
        return cnt_;
    }

    bool needDestruct() const
    {
        return cnt_ == 0;
    }

    template<typename T>
    T* allocMemory(size_t n)
    {
        if (curr_ == nullptr)
            addBlock();
        T *result = curr_->allocAligned<T>(n);
        if (result != nullptr)
            return result;
        addBlock();
        return curr_->allocAligned<T>(n);
    }
private:
    size_t cnt_;
    Block *curr_;

    void addBlock()
    {
        curr_ = new Block(curr_);
    }
};

template<typename T>
class StackAllocator
{
public:
    template<typename U>
    friend class StackAllocator;

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;

    StackAllocator()
    {
        mb_ = new MemoryBlocks();
        mb_->addReference();
    }

    template<typename U>
    StackAllocator(const StackAllocator<U> &other) : mb_(other.mb_)
    {
        mb_->addReference();
    }

    template<typename U>
    StackAllocator<T>& operator=(const StackAllocator &other)
    {
        mb_ = other.mb_;
        mb_->addReference();
    }

    ~StackAllocator()
    {
        mb_->eraseReference();
        if (mb_->needDestruct())
            delete mb_;
    }

    template<typename U>
    struct rebind
    {
        using other = StackAllocator<U>;
    };

    pointer allocate(size_t n)
    {
        return mb_->allocMemory<T>(n);
    }

    void deallocate(pointer ptr, size_t n) {}
private:
    MemoryBlocks *mb_;
};
