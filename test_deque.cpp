# include <iostream>
# include <vector>
# include <fstream>
# include <cstdlib>
# include <ctime>
# include <gtest/gtest.h>
# include "deque.h"

template<typename T>
class SillyDeque
{
private:
    std::vector<T> elems;
public:
    void push_back(const T &x)
    {
        elems.push_back(x);
    }

    void push_front(const T &x)
    {
        elems.insert(elems.begin(), x);
    }

    T pop_back()
    {
        T ans = elems.back();
        elems.pop_back();
        return ans;
    }

    T pop_front()
    {
        T ans = elems.front();
        elems.erase(elems.begin());
        return ans;
    }

    size_t size() const
    {
        return elems.size();
    }

    bool empty() const
    {
        return elems.empty();
    }

    T& back()
    {
        return elems.back();
    }

    const T& back() const
    {
        return elems.back();
    }

    T& front()
    {
        return elems.front();
    }

    const T& front() const
    {
        return elems.front();
    }

    T& operator[](const int &index)
    {
        return elems[index];
    }

    const T& operator[](const int &index) const
    {
        return elems[index];
    }
};

template<typename T>
bool check(const Deque<T> &d, const SillyDeque<T> &sd)
{
    if (d.size() != sd.size()) return false;
    if (d.empty() != sd.empty()) return false;
    if (!d.empty() && (d.back() != sd.back())) return false;
    if (!d.empty() && (d.front() != sd.front())) return false;
    typename Deque<T>::const_iterator it = d.begin();
    typename Deque<T>::const_reverse_iterator rit = --d.rend();
    for (size_t i = 0; i < d.size(); i++)
    {
        if (d[i] != sd[i]) return false;
        if (sd[i] != *it || sd[i] != *rit) return false;
        ++it; --rit;
    }
    return true;
}

template<typename T>
void randomOperation(Deque<T> &d, SillyDeque<T> &sd, const T &random_value)
{
    int operation = rand() % 4;
    switch(operation)
    {
    case 0:
        d.push_back(random_value);
        sd.push_back(random_value);
    case 1:
        d.push_front(random_value);
        sd.push_front(random_value);
    case 2:
        if (!d.empty())
        {
            d.pop_back();
            sd.pop_back();
        }
    case 3:
        if (!d.empty())
        {
            d.pop_front();
            sd.pop_front();
        }
    }
}

TEST(DequeTest, PushPopTest)
{
    Deque<int> d;
    SillyDeque<int> sd;
    EXPECT_TRUE(check(d, sd));
    for (int i = 0; i < 1000; i++)
    {
        randomOperation(d, sd, rand());
        EXPECT_TRUE(check(d, sd));
    }
}

TEST(DequeTest, TimeTest)
{
    std::ofstream out("output.txt");
    for (int size = 1e5; size <= 5e7; size *= 1.05)
    {
        out << size << " ";
        std::clock_t start = std::clock();
        Deque<int> d;
        for (int i = 0; i < size; i++)
        {
            if (d.empty() || !rand() % 3)
            {
                d.push_back(rand());
            }
            else
            {
                d.pop_front();
            }
        }
        std::clock_t finish = std::clock();
        out << (double)(finish - start) / CLOCKS_PER_SEC << "\n";
    }
    out.close();
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
