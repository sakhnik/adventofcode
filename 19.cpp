#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <fstream>

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

std::string Trace(const RouteT &route)
{
    std::string trace;
    int row{0};
    int col = route[row].find('|');
    int dx{0}, dy{1};

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
        }

        row += dy;
        col += dx;

        if (row < 0 || size_t(row) >= route.size())
            break;
        if (col < 0 || size_t(col) >= route[0].size())
            break;
    }

    return trace;
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
    REQUIRE(Trace(route) == "ABCDEF");
}

TEST_CASE("main")
{
    auto route = Parse(std::ifstream(INPUT));
    std::cout << Trace(route) << std::endl;
}
