#include "../test.hpp"

namespace {

class Crabs
{
public:
    Crabs(std::istream &&is)
    {
        std::string line;
        getline(is, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream iss(line);
        int n{};
        while (iss >> n)
            _positions.push_back(n);
    }

    int FindAlignment()
    {
        std::sort(_positions.begin(), _positions.end());

        auto min_fuel = std::accumulate(_positions.begin(), _positions.end(), 0);
        auto fuel = min_fuel;
        auto it = _positions.begin();
        for (int x = 0; x <= _positions.back(); ++x)
        {
            while (it != _positions.end() && *it == x)
                ++it;
            auto count_left = it - _positions.begin();
            auto count_right = _positions.end() - it;
            fuel += count_left - count_right;
            if (fuel < min_fuel)
                min_fuel = fuel;
        }

        return min_fuel;
    }

private:
    std::vector<int> _positions;
};

const char *TEST_INPUT = "16,1,2,0,4,2,7,1,2,14";

using namespace boost::ut;

suite s = [] {
    "2021-07"_test = [] {
        Crabs test(std::istringstream{TEST_INPUT});
        expect(37_i == test.FindAlignment());

        Crabs crabs(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", crabs.FindAlignment());
    };
};

} //namespace;
