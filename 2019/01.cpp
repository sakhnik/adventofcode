#include <doctest/doctest.h>
#include <fstream>
#include <iterator>
#include <numeric>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    using IterT = std::istream_iterator<int>;
    auto fuel = std::accumulate(IterT{ifs}, IterT{}, 0,
                                [](auto fuel, auto mass) {
                                    return fuel + mass / 3 - 2;
                                });
    MESSAGE(fuel);
}
