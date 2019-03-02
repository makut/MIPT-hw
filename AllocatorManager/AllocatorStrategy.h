# pragma once
# include <memory>

template<typename AllocStrategy>
class CAllocatedOn
{
public:
    void* operator new(size_t count)
    {
        return AllocStrategy::Alloc(count);
    }

    void operator delete(void *ptr)
    {
        AllocStrategy::Free(ptr);
    }
};

class RuntimeHeap
{
public:
    static void* Alloc(size_t count)
    {
        return std::malloc(count);
    }

    static void Free(void *ptr)
    {
        return std::free(ptr);
    }
};

class CurrentMemoryManager
{
public:
    static void* Alloc(size_t count)
    {
        return ::operator new(count);
    }

    static void Free(void *ptr)
    {
        ::operator delete(ptr);
    }
};