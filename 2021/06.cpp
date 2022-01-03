#include "../test.hpp"
#include <numeric>
#include <fstream>

namespace {

class Fish
{
public:
    Fish(std::istream &&is)
        : Fish()
    {
        std::string line;
        getline(is, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream iss(line);
        size_t n{};
        while (iss >> n)
            ++_counts[n];
    }

    Fish()
    {
        _counts.fill(0);
    }

    Fish SimulateOne() const
    {
        Fish next;
        for (size_t i = 1; i < _counts.size(); ++i)
            next._counts[i - 1] = _counts[i];
        next._counts[6] += _counts[0];
        next._counts[8] = _counts[0];
        return next;
    }

    Fish Simulate(size_t count) const
    {
        Fish next(*this);
        while (count--)
            next = next.SimulateOne();
        return next;
    }

    size_t CountTotal() const
    {
        return std::accumulate(_counts.begin(), _counts.end(), size_t{0});
    }

private:
    using _CountsT = std::array<size_t, 9>;
    _CountsT _counts;
};


const char *TEST_INPUT = "3,4,3,1,2";

using namespace boost::ut;

suite s = [] {
    "2021-06"_test = [] {
        Fish test_fish(std::istringstream{TEST_INPUT});
        expect(26_u == test_fish.Simulate(18).CountTotal());
        expect(5934_u == test_fish.Simulate(80).CountTotal());
        expect(eq(26984457539ull, test_fish.Simulate(256).CountTotal()));

        Fish fish(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", fish.Simulate(80).CountTotal());
        Printer::Print(__FILE__, "2", fish.Simulate(256).CountTotal());
    };
};

} //namespace;
