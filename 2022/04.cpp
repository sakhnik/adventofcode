#include "../test.hpp"
#include <fstream>

namespace {

const char *const TEST = R"(2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8
)";

int CountOverlaps(std::istream &&is)
{
    using RangeT = std::pair<int, int>;
    int count{};

    std::string line;
    while (std::getline(is, line))
    {
        RangeT a{}, b{};
        sscanf(line.c_str(), "%d-%d,%d-%d", &a.first, &a.second, &b.first, &b.second);
        if (a.first == b.first)
        {
            ++count;
            continue;
        }
        if (a.first > b.first)
            a.swap(b);
        if (a.second >= b.second)
            ++count;
    }
    return count;
}

using namespace boost::ut;

suite s = [] {
    "2022-04"_test = [] {
        expect(2_i == CountOverlaps(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", CountOverlaps(std::ifstream{INPUT}));
    };
};

} //namespace;
