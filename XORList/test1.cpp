#include "XorList.h"
#include <gtest/gtest.h>
#include <iterator>
#include <list>
# include <algorithm>
#include <random>

TEST(XorListTest, DefaultConstructor)
{
    XorList<int> mlist;
    ASSERT_TRUE(mlist.empty());
}

TEST(XorListTest, InitializerConstructor) 
{
    XorList<int> mlist1{1, 2, 3, 4, 5};
    std::list<int> mlist2{1, 2, 3, 4, 5};
    ASSERT_TRUE(std::equal(mlist1.begin(), mlist1.end(), mlist2.begin()));
}

TEST(XorListTest, CopyConstructor)
{
    XorList<int> mlist1{1, 2, 3, 4, 5};
    XorList<int> mlist2(mlist1);
    ASSERT_TRUE(std::equal(mlist1.begin(), mlist1.end(), mlist2.begin()));
}

TEST(XorListTest, MoveConstructor) 
{
    XorList<int> mlist1{1, 2, 3, 4, 5};
    XorList<int> mlist2(mlist1);
    XorList<int> mlist3(std::move(mlist2));
    ASSERT_TRUE(mlist2.empty());
    ASSERT_TRUE(std::equal(mlist1.begin(), mlist1.end(), mlist3.begin()));
}

TEST(XorListTest, PushBackandPopBack)
{
    XorList<int> mlist;
    for (int i = 0; i < 10; ++i)
        mlist.push_back(i);
    for (auto it = mlist.begin(); it!=mlist.end(); ++it)
    {
        static int i = 0;
        ASSERT_EQ(*it, i);
        ++i;
    }
}

TEST(XorListTest, PushFrontandPopFront)
{
    XorList<int> mlist;
    for (size_t i = 0; i < 10; ++i)
    {
        if (i%2)
            mlist.push_back(i);
        else
            mlist.push_front(i);
    }
    std::vector<int> test_vec{8, 6, 4, 2, 0, 1, 3, 5, 7, 9};
    for (auto it = mlist.begin(); it!=mlist.end(); ++it)
    {
        static int i = 0;
        ASSERT_EQ(test_vec[i], *it);
        ++i;
    }
    while (!mlist.empty())
        mlist.pop_front();
    ASSERT_EQ(mlist.size(), 0);
}

TEST(XorListTest, Operations)
{
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<int> num_distr(std::numeric_limits<int>::min());
    std::uniform_int_distribution<int> op_distr(0, 3);
    XorList<int> mlist1;
    std::list<int> mlist2;
    enum OperationTp { PushBack, PushFront, PopBack, PopFront };
    for (int i = 0; i < 1000000; ++i)
    {
        OperationTp op = static_cast<OperationTp>(op_distr(gen));
        if (mlist2.empty() && (op==PopBack || op==PopFront))
        {
            --i;
            continue;
        }
        switch (op)
        {
            case PushBack:
            {
                int cur = num_distr(gen);
                mlist1.push_back(cur);
                mlist2.push_back(cur);
                break;
            }
            case PushFront:
            {
                int cur = num_distr(gen);
                mlist1.push_front(cur);
                mlist2.push_front(cur);
                break;
            }
            case PopBack:
            {
                mlist1.pop_back();
                mlist2.pop_back();
                break;
            }
            case PopFront:
            {
                mlist1.pop_front();
                mlist2.pop_front();
                break;
            }
    }
  }
  ASSERT_TRUE(std::equal(mlist1.begin(), mlist1.end(), mlist2.begin()));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}