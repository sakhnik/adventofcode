#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

typedef std::vector<std::string> RouteT;

RouteT Parse(std::istream &is)
{
    RouteT route;
    std::string line;
    while (getline(is, line))
    {
        route.push_back(line);
    }
    return route;
}

RouteT Parse(std::istream &&is)
{
    return Parse(is);
}

struct Result
{
    std::string trace;
    unsigned steps;
};

Result Trace(const RouteT &route)
{
    std::string trace;
    int row{0};
    int col = route[row].find('|');
    int dx{0}, dy{1};
    unsigned steps{0};

    while (true)
    {
        switch (char ch = route[row][col])
        {
        case '+':
            {
                // change direction
                if (size_t(row + 1) < route.size() && route[row+1][col] != ' ' && dy != -1)
                {
                    dx = 0;
                    dy = 1;
                }
                else if (route[row-1][col] != ' ' && dy != 1)
                {
                    dx = 0;
                    dy = -1;
                }
                else if (route[row][col+1] != ' ' && dx != -1)
                {
                    dx = 1;
                    dy = 0;
                }
                else if (route[row][col-1] != ' ' && dx != 1)
                {
                    dx = -1;
                    dy = 0;
                }
                else
                    REQUIRE(!"Not implemented");
                break;
            }
        case 'A'...'Z':
            trace += ch;
            break;
        case ' ':
            return {trace, steps};
        }

        row += dy;
        col += dx;
        ++steps;
    }

    return {trace, steps};
}

TEST_CASE("test")
{
    const char *const test =
"     |          \n"
"     |  +--+    \n"
"     A  |  C    \n"
" F---|----E|--+ \n"
"     |  |  |  D \n"
"     +B-+  +--+ \n"
    ;
    auto route = Parse(std::istringstream(test));
    auto result = Trace(route);
    REQUIRE(result.trace == "ABCDEF");
    REQUIRE(result.steps == 38);
}

TEST_CASE(TEST_NAME)
{
    auto route = Parse(std::ifstream(INPUT));
    auto result = Trace(route);
    std::cout << result.trace << std::endl;
    std::cout << result.steps << std::endl;
}
