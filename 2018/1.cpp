#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>

int Sum(std::istream &is)
{
    int res{};

    int i{};
    while (is && (is >> i))
    {
        res += i;
    }

    return res;
}

int Sum(std::istream &&is)
{
    return Sum(is);
}

TEST_CASE("main")
{
    REQUIRE(Sum(std::istringstream{"+1 -2 +3 +1"}) == 3);
    REQUIRE(Sum(std::istringstream{"+1 +1 +1"}) == 3);
    REQUIRE(Sum(std::istringstream{"+1 +1 -2"}) == 0);
    REQUIRE(Sum(std::istringstream{"-1 -2 -3"}) == -6);

    std::cout << Sum(std::ifstream(INPUT)) << std::endl;
}
