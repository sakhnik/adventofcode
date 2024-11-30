#include <fstream>
#include <vector>
#include <sstream>
#include "../test.hpp"

namespace {

using namespace boost::ut;
using namespace std::string_literals;

using RouteT = std::vector<std::string>;

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
                    expect(false) << "Not implemented";
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

suite s = [] {
    "19"_test = [] {
        {
            const char *const test =
                "     |          \n"
                "     |  +--+    \n"
                "     A  |  C    \n"
                " F---|----E|--+ \n"
                "     |  |  |  D \n"
                "     +B-+  +--+ \n";
            auto route = Parse(std::istringstream(test));
            auto result = Trace(route);
            expect(eq(result.trace, "ABCDEF"s));
            expect(result.steps == 38_u);
        }

        auto route = Parse(std::ifstream(INPUT));
        auto result = Trace(route);
        Printer::Print(__FILE__, "1", result.trace);
        Printer::Print(__FILE__, "2", result.steps);
    };
};

} //namespace;
