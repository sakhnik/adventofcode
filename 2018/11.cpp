#include <doctest/doctest.h>
#include <limits>
#include <sstream>
#include <iostream>
#include <vector>

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

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("level") {
        REQUIRE(-5 == Grid::CalcLevel(122, 79, 57));
        REQUIRE(0 == Grid::CalcLevel(217, 196, 39));
        REQUIRE(4 == Grid::CalcLevel(101, 153, 71));
    }

    SUBCASE("search") {
        REQUIRE("33,45" == Grid{18}.FindMaxPower3());
        REQUIRE("21,61" == Grid{42}.FindMaxPower3());
    }

    SUBCASE("task1") {
        MESSAGE(Grid{1723}.FindMaxPower3());
    }

    SUBCASE("size") {
        REQUIRE("90,269,16" == Grid{18}.FindMaxPower());
        REQUIRE("232,251,12" == Grid{42}.FindMaxPower());
    }

    SUBCASE("task2") {
        MESSAGE(Grid{1723}.FindMaxPower());
    }
}
