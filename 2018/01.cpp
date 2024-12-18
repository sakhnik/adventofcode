#include <fstream>
#include <unordered_set>
#include <iterator>
#include <numeric>
#include <vector>
#include <sstream>
#include "../test.hpp"
namespace {

using TuneT = std::vector<int>;

TuneT Parse(std::istream &&is)
{
    return {std::istream_iterator<int>{is}, std::istream_iterator<int>{}};
}

TuneT GetInput()
{
    return Parse(std::ifstream{INPUT});
}

TuneT GetSample(const char *s)
{
    return Parse(std::istringstream{s});
}

int Sum(const TuneT &tune)
{
    return std::accumulate(begin(tune), end(tune), 0);
}

int Twice(const TuneT &tune)
{
    std::unordered_set<int> sums{0};
    int acc{};
    while (true)
    {
        for (auto i : tune)
        {
            acc += i;
            if (!sums.insert(acc).second)
            {
                return acc;
            }
        }
    }
}

using namespace boost::ut;

suite s = [] {
    "01"_test = [] {
        expect(3_i == Sum(GetSample("+1 -2 +3 +1")));
        expect(3_i == Sum(GetSample("+1 +1 +1")));
        expect(0_i == Sum(GetSample("+1 +1 -2")));
        expect(-6_i == Sum(GetSample("-1 -2 -3")));

        auto input = GetInput();
        Printer::Print(__FILE__, "1", Sum(input));

        expect(0_i == Twice(GetSample("+1 -1")));
        expect(10_i == Twice(GetSample("+3 +3 +4 -2 -4")));
        expect(5_i == Twice(GetSample("-6 +3 +8 +5 -6")));
        expect(14_i == Twice(GetSample("+7 +7 -2 -7 -4")));

        Printer::Print(__FILE__, "2", Twice(input));
    };
};

} //namespace;
