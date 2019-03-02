# pragma once
# include <cstddef>
# include <memory>

// class RuntimeHeap
// {
// public:
//     static void* Alloc(size_t count)
//     {
//         return std::malloc(count);
//     }

//     static void Free(void *ptr)
//     {
//         return std::free(ptr);
//     }
// };

// class CurrentMemoryManager
// {
// public:
//     static void* Alloc(size_t count)
//     {
//         return ::operator new(count);
//     }

//     static void Free(void *ptr)
//     {
//         ::operator delete(ptr);
//     }
// };

// class IMemoryManager
// {
// public:
//     virtual void* Alloc(size_t size) = 0;
//     virtual void Free(void *ptr) = 0;

//     virtual ~IMemoryManager() = default;
// };

// class DefaultManager : public IMemoryManager
// {
// public:
//     virtual void* Alloc(size_t size) override
//     {
//         return std::malloc(size);
//     }

//     virtual void Free(void *ptr) override
//     {
//         std::free(ptr);
//     }
// };

// class AllocList;
// class CMemoryManagerSwitcher;
// void* operator new(size_t count);

// class AllocNode : public RuntimeHeap
// {
// public:
//     friend AllocList;
//     explicit AllocNode(IMemoryManager *alloc, AllocNode *prev = nullptr):
//         alloc_(alloc), prev_(prev) {}

//     ~AllocNode()
//     {
//         if (prev_ == nullptr)
//         {
//             alloc_->~IMemoryManager();
//             std::free(alloc_);
//         }
//     }
// private:
//     IMemoryManager *alloc_;
//     AllocNode *prev_;

//     void* Alloc(size_t size)
//     {
//         return alloc_->Alloc(size);
//     }

//     void Free(void *ptr)
//     {
//         alloc_->Free(ptr);
//     }
// };

// class AllocList
// {
// public:
//     friend CMemoryManagerSwitcher;
//     friend void* operator new(size_t count);

//     AllocList(): curr_(nullptr)
//     {
//         DefaultManager *dm = static_cast<DefaultManager*>(std::malloc(sizeof(DefaultManager)));
//         new(dm) DefaultManager();
//         curr_ = static_cast<AllocNode*>(std::malloc(sizeof(AllocNode)));
//         new(curr_) AllocNode(dm);
//     }

//     ~AllocList()
//     {
//         while (curr_->prev_ != nullptr)
//             popAlloc();
//         curr_->~AllocNode();
//         std::free(curr_);

//     }

//     void* Alloc(size_t size)
//     {
//         return curr_->Alloc(size);
//     }

// private:
//     AllocNode *curr_;

//     void pushAlloc(IMemoryManager *alloc)
//     {
//         curr_ = new AllocNode(alloc, curr_);
//     }

//     void popAlloc()
//     {
//         AllocNode *nxt = curr_->prev_;
//         delete curr_;
//         curr_ = nxt;
//     }

//     IMemoryManager* currentAllocator()
//     {
//         return curr_->alloc_;
//     }
// };

// class CMemoryManagerSwitcher
// {
// public:
//     friend void* operator new(size_t count);

//     explicit CMemoryManagerSwitcher(IMemoryManager *alloc)
//     {
//         alloc_.pushAlloc(alloc);
//     }

//     ~CMemoryManagerSwitcher()
//     {
//         alloc_.popAlloc();
//     }
// private:
//     static AllocList alloc_;
// };
// AllocList CMemoryManagerSwitcher::alloc_;

// void* operator new(size_t count)
// {
//     void *mem = CMemoryManagerSwitcher::alloc_.Alloc(alignof(std::max_align_t) + count);
//     uintptr_t alloc_mem(reinterpret_cast<uintptr_t>(CMemoryManagerSwitcher::alloc_.currentAllocator()));
//     new(mem) uintptr_t(alloc_mem);
//     return static_cast<char*>(mem) + alignof(std::max_align_t);
// }

// void operator delete(void *ptr)
// {
//     void *mem = static_cast<char*>(ptr) - alignof(std::max_align_t);
//     uintptr_t *alloc_mem = static_cast<uintptr_t*>(mem);
//     IMemoryManager *alloc = reinterpret_cast<IMemoryManager*>(*alloc_mem);
//     alloc->Free(mem);
// }

class Block
{
public:

    explicit Block(Block *previous = nullptr, size_t size = SIZE):
        begin_(new(std::malloc(size * sizeof(char))) char[size]), end_(begin_ + size), 
        curr_(reinterpret_cast<void*>(begin_)),
        space_(size), prev_(previous) {}

    ~Block()
    {
        std::free(begin_);
        if (prev_ != nullptr)
        {
            prev_->~Block();
            std::free(prev_);
        }
    }

    void* allocAligned(size_t size)
    {
        if (std::align(alignof(std::max_align_t), size, curr_, space_))
        {
            void *result = curr_;
            curr_ = reinterpret_cast<char*>(curr_) + size;
            space_ -= size;
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
        {
            curr_->~Block();
            std::free(curr_);
        }
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

    void* allocMemory(size_t size)
    {
        if (curr_ == nullptr)
            addBlock();
        void *result = curr_->allocAligned(size);
        if (result != nullptr)
            return result;
        addBlock();
        return curr_->allocAligned(size);
    }
private:
    size_t cnt_;
    Block *curr_;

    void addBlock()
    {

        curr_ = new(std::malloc(sizeof(Block))) Block(curr_);
    }
};

class StackAllocator : public IMemoryManager
{
public:
    StackAllocator()
    {
        mb_ = new(std::malloc(sizeof(MemoryBlocks))) MemoryBlocks();
        mb_->addReference();
    }

    ~StackAllocator()
    {
        mb_->eraseReference();
        if (mb_->needDestruct())
        {
            mb_->~MemoryBlocks();
            std::free(mb_);
        }
    }

    virtual void* Alloc(size_t size) override
    {
        return mb_->allocMemory(size);
    }

    virtual void Free(void *ptr) override {}
private:
    MemoryBlocks *mb_;
};

// template<typename AllocStrategy>
// class CAllocatedOn
// {
// public:
//     void* operator new(size_t count)
//     {
//         return AllocStrategy::Alloc(count);
//     }

//     void operator delete(void *ptr)
//     {
//         AllocStrategy::Free(ptr);
//     }
// };
