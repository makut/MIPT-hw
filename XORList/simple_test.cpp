# include "StackAllocator.h"
// # include "XorList.h"
# include <iostream>
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
    begin = std::chrono::steady_clock::now();
    auto own = processOperations<std::list<int, StackAllocator<int> >, Operations>(vec);
    end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration<double, std::milli>(end - begin).count() << "\n";
    assert(std::equal(standard.begin(), standard.end(), own.begin()));
}


int main()
{
    executeTest<10000000>();
    // std::mt19937 gen(time(0));
    // std::uniform_int_distribution<> distr(0, 100);
    // std::list<int> std_list;
    // std::list<int, StackAllocator<int> > own_list;
    // for (int i = 0; i < 100000; i++)
    // {
    //     int rnd = distr(gen);
    //     if (rnd <= 33 && !std_list.empty())
    //     {
    //         std_list.pop_back();
    //         own_list.pop_back();
    //     }
    //     else
    //     {
    //         std_list.push_front(i);
    //         own_list.push_front(i);
    //     }
    // }
    // auto std_it = std_list.begin();
    // auto own_it = own_list.begin();
    // while (std_it != std_list.end())
    // {
    //     if (*std_it != *own_it)
    //     {
    //         std::cout << *std_it << " " << *own_it << "\n";
    //         return 0;
    //     }
    //     ++std_it;
    //     ++own_it;
    // }
    // std::list<int, StackAllocator<int> > cpy(std::move(own_list));
    // return 0;
}