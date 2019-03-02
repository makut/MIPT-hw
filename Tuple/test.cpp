# include <iostream>
# include <gtest/gtest.h>
# include "Tuple.h"

TEST(TupleTest, Constructor)
{
    Tuple<int, int, double> mtpl(1, 1, 1.);
    EXPECT_EQ(get<0>(mtpl), 1);
    EXPECT_EQ(get<0>(mtpl), 1);
    EXPECT_EQ(get<0>(mtpl), 1.);
    Tuple<int, int, double> other(mtpl);
    EXPECT_EQ(mtpl, other);
    Tuple<int> t1(0);
    Tuple<int> t2(t1);
    EXPECT_EQ(t1, t2);
}
TEST(TupleTest, Assignment)
{
    Tuple<int, double, double> mtpl(1, 2.4, 2.5);
    get<1>(mtpl) = 2.3;
    EXPECT_EQ(get<1>(mtpl), 2.3);
}
TEST(TupleTest, GetByType)
{
    Tuple<int, double, double> mtpl(3, 2, 1);
    EXPECT_EQ(get<int>(mtpl), 3);
    EXPECT_EQ(get<double>(mtpl), 2.0); // <- static assertion failed.
}

TEST(TupleTest, TupleCat)
{
    // Tuple<> mt1;
    Tuple<int, int, double> mt1(1, 1, 1.);
    Tuple<float, float, int> mt2(1., 1., 1);
    EXPECT_TRUE(concatenateTwo(mt1, mt2) == makeTuple(1, 1, 1., 1., 1., 1));
    EXPECT_TRUE(tupleCat(makeTuple(1, 2, 3), makeTuple(4, 5, 6),
                         makeTuple(7, 8, 9, 10)) ==
                makeTuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
}
// 
TEST(TupleTest, ComparsionTest)
{
    Tuple<int, int, double> mt1(1, 1, 1.);
    Tuple<int, int, float> mt2(1, 1, 1.);
    Tuple<int, int, double> mt3(1, 2, 3.0);
    EXPECT_TRUE(mt1 == mt2);
    EXPECT_FALSE(mt1 != mt2);
    EXPECT_FALSE(mt1 < mt2);
    EXPECT_FALSE(mt1 > mt2);
    EXPECT_TRUE(mt1 <= mt2);
    EXPECT_TRUE(mt1 >= mt2);

    EXPECT_TRUE(mt1 != mt3);
    EXPECT_FALSE(mt1 == mt3);
    EXPECT_TRUE(mt1 < mt3);
    EXPECT_FALSE(mt1 > mt2);
    EXPECT_TRUE(mt1 <= mt3);
    EXPECT_FALSE(mt1 >= mt3);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}