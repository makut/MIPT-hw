#include "StackAllocator.h"
#include "XorList.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <iterator>
#include <list>
#include <random>
#include <vector>
template<typename _List>
_List process_operations(size_t n1, size_t n2, const std::vector<int> &mvec) {
    _List mlist;
    auto it = mvec.begin();
    for (size_t i = 0; i < n1; ++i) {
        mlist.push_back(*it);
        ++it;
    }
    for (size_t i = 0; i < n2; ++i) {
      bool del = *it%2;
      ++it;
      if (del && !mlist.empty())
        mlist.pop_back();
      else {
        mlist.push_back(*it);
        ++it;
      }
    }
    return mlist;
}
template<template<class, class> class _Container>
void process_sample(size_t n1, size_t n2, const std::string &str) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr;
    std::vector<int> mvec(n1 + 2*n2);
    std::generate(mvec.begin(), mvec.end(),
                  [&distr, &gen]() { return distr(gen); });
    auto begin = std::chrono::steady_clock::now();
    auto l1 =
        process_operations<_Container<int, std::allocator<int>>>(n1, n2, mvec);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Standard allocator " + str << ":\t"
              << std::chrono::duration<double, std::milli>(end - begin).count()
              << " ms\n";
    begin = std::chrono::steady_clock::now();
    auto l2 =
        process_operations<_Container<int, StackAllocator<int>>>(n1, n2, mvec);
    end = std::chrono::steady_clock::now();
    std::cout << "Stack allocator " + str << ":\t"
              << std::chrono::duration<double, std::milli>(end - begin).count()
              << " ms\n";
    assert(std::equal(l1.begin(), l1.end(), l2.begin()));
}
int main() {
  size_t n1, n2;
  std::cin >> n1 >> n2;
  process_sample<std::list>(n1, n2, "list");
  process_sample<XorList>(n1, n2, "xorlist");
  return 0;
}