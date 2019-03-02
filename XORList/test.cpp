# include <gtest/gtest.h>
# include <random>
# include <list>
# include <ctime>
# include "StackAllocator.h"
# include "XorList.h"

template<typename Allocator, typename T = int, typename List = std::list<T, Allocator>, int DeleteChance = 0, int Operations = 10000>
class Tester
{
private:
    template<typename Lst>
    class TestResult;

    class Test;

    enum Operation {Insert, Delete};
public:
    template<typename Lst>
    static TestResult<Lst> determinedTest()
    {
        Lst lst;
        clock_t start = clock();
        for (size_t i = 0; i < Operations; i++)
        {
            Operation op = getRandomOperation();
            if (op == Delete && !lst.empty())
                lst.pop_back();
            else
                lst.push_front(i);
        }
        clock_t finish = clock();
        TestResult<Lst> result(start, finish, std::move(lst));
        return std::move(result);
    }

    static Test executeTest()
    {
        TestResult<std::list<T> > std_res = determinedTest<std::list<T> >();
        TestResult<List> lst_res(determinedTest<List>());
        Test tst(std::move(lst_res), std::move(std_res));
        return std::move(tst);
    }

    static void speedTest()
    {
        Test test(executeTest());
        std::cout << "User list time in clocks: " << test.getOwnRes().getTestTime() << "\n";
        std::cout << "Standart list time in clocks: " << test.getStdRes().getTestTime() << "\n";
    }
private:
    static std::mt19937 gen_;
    static std::uniform_int_distribution<> distr_;

    template<typename Lst>
    class TestResult
    {
    public:
        friend Tester;

        TestResult() = default;

        TestResult(clock_t start, clock_t finish, Lst&& res):
            start_(start), finish_(finish), result_(std::move(res)) {}

        TestResult(const TestResult &other) = default;

        TestResult(TestResult&& other):
            start_(other.start_), finish_(other.finish_), result_(std::move(other.result_)) {}

        ~TestResult() = default;

        clock_t getTestTime() const
        {
            return finish_ - start_;
        }

        const Lst& getLstRes() const
        {
            return result_;
        }
    private:
        clock_t start_, finish_;
        Lst result_;
    };

    class Test
    {
    public:
        Test() = delete;

        Test(const Test &other) = default;

        Test(Test&& other):
            own_res_(std::move(other.own_res_)), std_res_(std::move(other.std_res_)) {}

        Test(const TestResult<List> &own, const TestResult<std::list<T>> &standart):
            own_res_(own), std_res_(standart) {}

        Test(TestResult<List>&& own, TestResult<std::list<T> >&& standart):
            own_res_(std::move(own)), std_res_(std::move(standart)) {}

        ~Test() = default;

        const TestResult<List>& getOwnRes() const
        {
            return own_res_;
        }

        const TestResult<std::list<T>>& getStdRes() const
        {
            return std_res_;
        }
    private:
        TestResult<List> own_res_;
        TestResult<std::list<T> > std_res_;
    };

    static Operation getRandomOperation()
    {
        int rnd_num = distr_(gen_);
        if (rnd_num <= DeleteChance)
            return Delete;
        return Insert;
    }
};
template<typename Allocator, typename T, typename List, int DeleteChance, int Operations>
std::mt19937 Tester<Allocator, T, List, DeleteChance, Operations>::gen_(time(0));
template<typename Allocator, typename T, typename List, int DeleteChance, int Operations>
std::uniform_int_distribution<> Tester<Allocator, T, List, DeleteChance, Operations>::distr_(0, 100);

TEST(StackAllocator, SpeedTest)
{
    Tester<StackAllocator<int>, int>::speedTest();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}