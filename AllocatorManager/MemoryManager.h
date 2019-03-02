# pragma once
# include "AllocatorStrategy.h"
# include <cstdlib>
# include <new>
# include <memory>
# include <cstddef>

class IMemoryManager
{
public:
    virtual void* Alloc(size_t size) = 0;
    virtual void Free(void *ptr) = 0;

    virtual ~IMemoryManager() = default;
};

class DefaultManager : public IMemoryManager
{
public:
    virtual void* Alloc(size_t size) override
    {
        return std::malloc(size);
    }

    virtual void Free(void *ptr) override
    {
        std::free(ptr);
    }
};

void* operator new(size_t count);
void operator delete(void *ptr) noexcept;

class CMemoryManagerSwitcher
{
public:
    friend void* operator new(size_t count);
    friend void operator delete(void *ptr) noexcept;

    explicit CMemoryManagerSwitcher(IMemoryManager *alloc) : curr_(alloc), prev_(top_)
    {
        top_ = this;
    }

    ~CMemoryManagerSwitcher()
    {
        top_ = prev_;
        curr_ = nullptr;
        prev_ = nullptr;
    }
private:
    IMemoryManager *curr_;
    CMemoryManagerSwitcher *prev_;
    static CMemoryManagerSwitcher *top_;
    static size_t align_;
};