#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <boost/ut.hpp>

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

using namespace boost::ut;

suite s = [] {
    "2015-01"_test = [] {
        expect(0_i == Count("(())"));
        expect(3_i == Count("((("));
        expect(3_i == Count("(()(()("));
        expect(3_i == Count("))((((("));
        expect(-1_i == Count("())"));
        expect(-1_i == Count("))("));
        expect(-3_i == Count(")))"));
        expect(-3_i == Count(")())())"));

        std::ifstream ifs{INPUT};
        std::string input((std::istreambuf_iterator<char>(ifs)),
                          std::istreambuf_iterator<char>());
        std::cout << "2015-01.1: " << Count(input) << std::endl;

        expect(1_i == Position(")"));
        expect(5_i == Position("()())"));

        std::cout << "2015-01.2: " << Position(input) << std::endl;
    };
};

} //namespace;