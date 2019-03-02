# include <iostream>
# include "Tuple.h"

int main()
{
    int a = 5;
    Tuple<int, int, int&> t(1, 2, a);
    ++(get<2>(t));
    std::cout << a << "\n";
}