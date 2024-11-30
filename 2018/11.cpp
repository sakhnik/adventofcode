#include <limits>
#include <sstream>
#include <vector>
#include "../test.hpp"

namespace {

class Grid
{
public:
    static const int W = 301;

    Grid(int serial)
        : _serial(serial)
        , _summed_areas(W*W)
    {
        // Precalculate the summed area table
        for (int i = 0; i < W; ++i)
        {
            _summed_areas[_GetIdx(i, 0)] = 0;
            _summed_areas[_GetIdx(0, i)] = 0;
        }

        for (int j = 1; j < W; ++j)
        {
            for (int i = 1; i < W; ++i)
            {
                auto s = CalcLevel(i, j, _serial);
                s += _summed_areas[_GetIdx(i-1, j)];
                s += _summed_areas[_GetIdx(i, j-1)];
                s -= _summed_areas[_GetIdx(i-1, j-1)];
                _summed_areas[_GetIdx(i, j)] = s;
            }
        }
    }

    std::string FindMaxPower3() const
    {
        auto ret = _FindMaxPower(3);
        std::ostringstream oss;
        oss << ret.x << "," << ret.y;
        return oss.str();
    }

    std::string FindMaxPower() const
    {
        MaxPower ret{};
        int size{};

        for (int s = 1; s < W; ++s)
        {
            auto p = _FindMaxPower(s);
            if (p.power > ret.power)
            {
                ret = p;
                size = s;
            }
        }

        std::ostringstream oss;
        oss << ret.x << ',' << ret.y << ',' << size;
        return oss.str();
    }

    static int CalcLevel(int x, int y, int serial)
    {
        int rack = x + 10;
        int power = rack * y;
        power += serial;
        power *= rack;
        power /= 100;
        power %= 10;
        return power - 5;
    }

private:
    int _serial;
    std::vector<int8_t> _levels;
    std::vector<int> _summed_areas;

    static size_t _GetIdx(int x, int y)
    {
        return x + y*W;
    }

    struct MaxPower
    {
        int x, y;
        int power = std::numeric_limits<int>::min();
    };

    MaxPower _FindMaxPower(int size) const
    {
        // To avoid repetitive calculations, let's implement a sliding window.
        MaxPower ret{};
        auto checkPower = [&](int i, int j, int power) {
            if (power > ret.power)
            {
                ret.x = i - size + 1;
                ret.y = j - size + 1;
                ret.power = power;
            }
        };

        for (int j = size; j < W; ++j)
        {
            for (int i = size; i < W; ++i)
            {
                auto power = _CalcSquare(i, j, size);
                checkPower(i, j, power);
            }
        }

        return ret;
    }

    int _CalcSquare(int i, int j, int s) const
    {
        int power = _summed_areas[_GetIdx(i, j)];
        power -= _summed_areas[_GetIdx(i - s, j)];
        power -= _summed_areas[_GetIdx(i, j - s)];
        power += _summed_areas[_GetIdx(i - s, j - s)];
        return power;
    }
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "11"_test = [] {
        // level
        expect(-5_i == Grid::CalcLevel(122, 79, 57));
        expect(0_i == Grid::CalcLevel(217, 196, 39));
        expect(4_i == Grid::CalcLevel(101, 153, 71));

        // search
        expect(eq("33,45"s, Grid{18}.FindMaxPower3()));
        expect(eq("21,61"s, Grid{42}.FindMaxPower3()));

        Printer::Print(__FILE__, "1", Grid{1723}.FindMaxPower3());

        // size
        expect(eq("90,269,16"s, Grid{18}.FindMaxPower()));
        expect(eq("232,251,12"s, Grid{42}.FindMaxPower()));

        Printer::Print(__FILE__, "2", Grid{1723}.FindMaxPower());
    };
};

} //namespace;
