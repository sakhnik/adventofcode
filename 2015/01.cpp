#include <doctest/doctest.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>

namespace {

int Count(const std::string_view &s)
{
    auto ups = std::count(begin(s), end(s), '(');
    auto downs = std::count(begin(s), end(s), ')');
    return ups - downs;
}

int Position(const std::string_view &s)
{
    int floor{0};
    for (size_t i = 0; i < size(s); ++i)
    {
        if (s[i] == '(')
        {
            ++floor;
        }
        else if (s[i] == ')')
        {
            --floor;
        }
        if (floor == -1)
        {
            return i + 1;
        }
    }
    return -1;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    REQUIRE(0 == Count("(())"));
    REQUIRE(3 == Count("((("));
    REQUIRE(3 == Count("(()(()("));
    REQUIRE(3 == Count("))((((("));
    REQUIRE(-1 == Count("())"));
    REQUIRE(-1 == Count("))("));
    REQUIRE(-3 == Count(")))"));
    REQUIRE(-3 == Count(")())())"));

    std::ifstream ifs{INPUT};
    std::string input((std::istreambuf_iterator<char>(ifs)),
                      std::istreambuf_iterator<char>());
    MESSAGE(Count(input));

    REQUIRE(1 == Position(")"));
    REQUIRE(5 == Position("()())"));

    MESSAGE(Position(input));
}
