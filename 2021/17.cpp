#include "../test.hpp"

namespace {

class Probe
{
public:
    Probe(std::istream &&is)
    {
        std::string line;
        getline(is, line);
        sscanf(line.c_str(), "target area: x=%d..%d, y=%d..%d",
                &target_x.first, &target_x.second, &target_y.first, &target_y.second);
    }

    const static auto MISS = std::numeric_limits<int>::min();

    // Returns max height if hits, or MISS if misses.
    int Shoot(int dx, int dy) const
    {
        int max_y{0};

        int x{}, y{};
        while (true)
        {
            x += dx;
            y += dy;
            if (y > max_y)
                max_y = y;
            if (dx > 0)
                --dx;
            else if (dx < 0)
                ++dx;
            --dy;
            if (x > target_x.second && y > target_y.second)
                return MISS;
            if (x >= target_x.first && y >= target_y.first
                    && x <= target_x.second && y <= target_y.second)
                return max_y;
        }
    }

    int FindHighest() const
    {
        // max height is the sum of arithmetic progression
        auto dy = -target_y.first;
        return dy * (dy - 1) / 2;
    }

    using RangeT = std::pair<int, int>;
    RangeT target_x{};
    RangeT target_y{};
};

const char *TEST_INPUT = R"(target area: x=20..30, y=-10..-5)";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-17"_test = [] {
        Probe test{std::istringstream{TEST_INPUT}};
        expect(3_i == test.Shoot(7, 2));
        expect(6_i == test.Shoot(6, 3));
        expect(0_i == test.Shoot(9, 0));
        expect(Probe::MISS == test.Shoot(17, -4));
        expect(45_i == test.Shoot(6, 9));
        expect(45_i == test.FindHighest());

        Probe probe{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", probe.FindHighest());
    };
};

} //namespace;
