# include <iostream>
# include "MemoryManager.h"
# include "MemoryManager.cpp"
# include "StackAllocator.h"
# include <random>
# include <ctime>
# include <vector>
# include <chrono>
# include <list>
# include <cassert>


template<typename Lst, int Operations>
Lst processOperations(const std::vector<int> &val)
{
    Lst lst;
    auto it = val.begin();
    for (size_t i = 0; i < Operations; i++)
    {
        if ((*it) % 3 == 0 && !lst.empty())
        {
            ++it;
            lst.pop_back();
        }
        else
        {
            ++it;
            lst.push_front(*it);
            ++it;
        }
    }
    return lst;
}


template<int Operations>
void executeTest()
{
    std::mt19937 gen(time(0));
    std::uniform_int_distribution<> distr;
    std::vector<int> vec(2 * Operations);
    for (size_t i = 0; i < 2 * Operations; i++)
        vec[i] = distr(gen);
    auto begin = std::chrono::steady_clock::now();
    auto standard = processOperations<std::list<int>, Operations>(vec);
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration<double, std::milli>(end - begin).count() << "\n";
}


int main()
{
    executeTest<10000>();
    StackAllocator sa;
    CMemoryManagerSwitcher own(&sa);
    executeTest<10000>();
    return 0;
}