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

std::pair<int, int> CountOverlaps(std::istream &&is)
{
    using RangeT = std::pair<int, int>;
    int full_overlap{};
    int any_overlap{};

    std::string line;
    while (std::getline(is, line))
    {
        RangeT a{}, b{};
        sscanf(line.c_str(), "%d-%d,%d-%d", &a.first, &a.second, &b.first, &b.second);
        if (a.first == b.first)
        {
            ++full_overlap;
            ++any_overlap;
            continue;
        }
        if (a.first > b.first)
            a.swap(b);
        if (a.second >= b.second)
            ++full_overlap;
        if (a.second >= b.first)
            ++any_overlap;
    }
    return {full_overlap, any_overlap};
}

using namespace boost::ut;

suite s = [] {
    "2022-04"_test = [] {
        auto test = CountOverlaps(std::istringstream{TEST});
        expect(2_i == test.first);
        expect(4_i == test.second);

        auto res = CountOverlaps(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", res.first);
        Printer::Print(__FILE__, "2", res.second);
    };
};

} //namespace;
