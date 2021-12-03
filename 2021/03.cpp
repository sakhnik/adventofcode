#include "../test.hpp"

namespace {

size_t CalcConsumption(std::istream &&is)
{
    std::vector<int> counts;
    size_t total{};
    std::string s;
    while (is >> s)
    {
        if (counts.empty())
            counts.resize(s.size());
        for (int i = 0; i < s.size(); ++i)
            counts[i] += s[i] - '0';
        ++total;
    }
    size_t gamma{}, epsilon{};
    for (int i = 0; i < counts.size(); ++i)
    {
        gamma = (gamma << 1) + (counts[i] > total / 2);
        epsilon = (epsilon << 1) + (counts[i] <= total / 2);
    }
    return gamma * epsilon;
}

using namespace boost::ut;

const char *const TEST_INPUT = R"(00100
11110
10110
10111
10101
01111
00111
11100
10000
11001
00010
01010)";

suite s = [] {
    "2021-03"_test = [] {
        expect(198_u == CalcConsumption(std::istringstream{TEST_INPUT}));
        Printer::Print(__FILE__, "1", CalcConsumption(std::ifstream{INPUT}));
    };
};

} //namespace;
