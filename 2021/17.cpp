#include "../test.hpp"
#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <fstream>

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

    int CalcFallTime(int dy, int y) const
    {
        // y = v + (v - 1) + (v - 2) + (v - 3) + ... + (v - t) = vt - t(t - 1)/2
        //
        // y = vt - t²/2 + t/2
        // 2y = 2vt - t² + t
        //
        // t² - (2v + 1)t + 2y = 0

        double a = 1.0;
        double b = -(2.0 * dy + 1);
        double c = 2.0 * y;

        double d = b * b - 4 * a * c;
        if (d < 0)
            return 0;
        d = std::sqrt(d);
        double x = (-b - d) / (a + a);
        if (x > 0)
        {
            double t{};
            if (std::modf(x, &t) < 1e-10)
                return t;
        }

        x = (-b + d) / (a + a);
        if (x < 0)
            return 0;
        double t{};
        if (std::modf(x, &t) < 1e-10)
            return t;
        return 0;
    }

    int CalcX(int dx, int t) const
    {
        auto t0 = std::min(dx, t);
        return (2 * dx - t0 + 1) * t0 / 2;
    }

    int CalcSpeeds() const
    {
        int dy_min = target_y.first;
        int dy_max = -target_y.first;

        using SpeedT = std::pair<int, int>;
        std::unordered_set<SpeedT, boost::hash<SpeedT>> speeds;

        for (int dy = dy_min; dy <= dy_max; ++dy)
        {
            for (int y = target_y.first; y <= target_y.second; ++y)
            {
                int t = CalcFallTime(dy, y);
                if (!t)
                    continue;
                for (int dx = 5; dx <= target_x.second; ++dx)
                {
                    auto x = CalcX(dx, t);
                    if (x >= target_x.first && x <= target_x.second)
                    {
                        speeds.insert({dx, dy});
                    }
                }
            }
        }
        return speeds.size();
    }

    using RangeT = std::pair<int, int>;
    RangeT target_x{};
    RangeT target_y{};
};

const char *TEST_INPUT = R"(target area: x=20..30, y=-10..-5)";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "17"_test = [] {
        Probe test{std::istringstream{TEST_INPUT}};
        expect(3_i == test.Shoot(7, 2));
        expect(6_i == test.Shoot(6, 3));
        expect(0_i == test.Shoot(9, 0));
        expect(Probe::MISS == test.Shoot(17, -4));
        expect(45_i == test.Shoot(6, 9));
        expect(45_i == test.FindHighest());
        expect(6_i == test.CalcFallTime(2, -3));
        expect(0_i == test.CalcFallTime(-4, -6));
        expect(2_i == test.CalcX(2, 1));
        expect(3_i == test.CalcX(2, 2));
        expect(3_i == test.CalcX(2, 4));
        expect(3_i == test.CalcX(2, 5));
        expect(112_i == test.CalcSpeeds());

        Probe probe{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", probe.FindHighest());
        Printer::Print(__FILE__, "2", probe.CalcSpeeds());
    };
};

} //namespace;
