# include <iostream>
# include "MemoryManager.h"
# include "StackAllocator.h"
# include <random>

int main()
{
    std::mt19937 gen(time(0));
    std::uniform_int_distribution<> distr(0, 100);
    std::list<int, StackAllocator<int> > std_list;
    for (int i = 0; i < 100000; i++)
    {
        int rnd = distr(gen);
        if (rnd <= 33 && !std_list.empty())
        {
            std_list.pop_back();
            own_list.pop_back();
        }
        else
        {
            std_list.push_front(i);
            own_list.push_front(i);
        }
    }
    auto std_it = std_list.begin();
    auto own_it = own_list.begin();
    while (std_it != std_list.end())
    {
        if (*std_it != *own_it)
        {
            std::cout << *std_it << " " << *own_it << "\n";
            return 0;
        }
        ++std_it;
        ++own_it;
    }
    return 0;
}