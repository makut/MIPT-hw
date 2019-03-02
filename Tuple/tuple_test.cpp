#include <iostream>
#include <vector>
#include <utility>
#include <cassert>
#include <functional>
#include <algorithm>


#include "Tuple.h"

void test_tuple() {

    {
        Tuple<int, std::string, long> tuple;
    }

    {
        int test_int = 5;
        std::string test_string = "Hello";
        Tuple<int, std::string> tuple(test_int, test_string);
    }

    {
        int test_int = 5;
        std::string test_string = "Hello";
        std::vector<int> test_vector(10, 5);
        Tuple<int, std::string, std::vector<int>> tuple(test_int, test_string, std::move(test_vector));
    }

    {
        std::string test_string = "Hello";
        std::vector<int> test_vector(10, 5);
        Tuple<std::string, std::vector<int>> tuple(std::move(test_string), std::move(test_vector));
    }

    {
        Tuple<int> first(5);
        auto second = first;
        first.swap(second);

        auto third = std::move(first);

        assert(third == second);
    }

    {
        Tuple<int> first(12);
        auto second = first;
        assert(get<0>(first) == get<0>(second));

        second = Tuple<int>(14);
        assert(get<0>(second) == 14);

        first.swap(second);
        assert(get<0>(second) == 12);
        assert(get<0>(first) == 14);

        int&& test_int = get<0>(std::move(first));
        assert(test_int == 14);
    }

    {
        Tuple<int, long> tuple(12, 16);
        assert(get<int>(tuple) == 12);
        assert(get<long>(tuple) == 16);

        int test_int_lv = get<int>(tuple);
        int&& test_int_rv = get<int>(std::move(tuple));
        assert(test_int_lv == test_int_rv);
    }

    {
        Tuple<int, std::string, std::vector<int>> tuple = makeTuple(5, std::string("test"), std::vector<int>(2, 5));
        get<2>(tuple)[1] = 2;
        assert(get<2>(tuple)[1] == 2);
    }

    {
        int test_int = 1;
        auto tuple = makeTuple(test_int, std::ref(test_int));
        test_int = 2;
        assert(get<0>(tuple) == 1);
        assert(get<1>(tuple) == 2);
    }

    {
        std::vector<std::tuple<int, std::string, float>> v;
        v.emplace_back(2, "baz", -0.1);
        v.emplace_back(2, "bar", 3.14);
        v.emplace_back(1, "foo", 100.1);
        std::sort(v.begin(), v.end());

        assert(get<0>(v[0]) == 1);
        assert(get<0>(v[1]) == 2);
        assert(get<0>(v[2]) == 2);

        assert(get<1>(v[0]) == std::string("foo"));
        assert(get<1>(v[1]) == std::string("bar"));
        assert(get<1>(v[2]) == std::string("baz"));
    }

    {
        auto tuple = makeTuple(1, "hello");
        auto test_tuple = tupleCat(tuple, tuple, makeTuple(5, 10));

        assert(get<0>(test_tuple) == get<2>(test_tuple));
        assert(get<4>(test_tuple) == 5);
    }


    for (int i = 0; i < 10000; ++i) {
        Tuple<int, std::vector<int>> tuple(4, std::vector<int>(10000, 5));
        assert(get<int>(tuple) == 4);
    }

}

int main() {
    test_tuple();
    std::cout << 0 << "\n";
    return 0;
}
