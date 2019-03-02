# include "MemoryManager.h"

CMemoryManagerSwitcher *CMemoryManagerSwitcher::top_ = nullptr;
size_t CMemoryManagerSwitcher::align_ = std::max(alignof(std::max_align_t), sizeof(IMemoryManager*));

void* operator new(size_t count)
{
    IMemoryManager *curr;
    if (CMemoryManagerSwitcher::top_ == nullptr)
        curr = nullptr;
    else
        curr = CMemoryManagerSwitcher::top_->curr_;
    void *mem;
    if (curr == nullptr)
        mem = std::malloc(CMemoryManagerSwitcher::align_ + count);
    else
        mem = curr->Alloc(CMemoryManagerSwitcher::align_ + count);
    uintptr_t alloc_mem(reinterpret_cast<uintptr_t>(curr));
    new(mem) uintptr_t(alloc_mem);
    return static_cast<char*>(mem) + CMemoryManagerSwitcher::align_;
}

void operator delete(void *ptr) noexcept
{
    void *mem = static_cast<char*>(ptr) - CMemoryManagerSwitcher::align_;
    uintptr_t *alloc_mem = static_cast<uintptr_t*>(mem);
    IMemoryManager *alloc = reinterpret_cast<IMemoryManager*>(*alloc_mem);
    if (alloc == nullptr)
        std::free(mem);
    else
        alloc->Free(mem);
}