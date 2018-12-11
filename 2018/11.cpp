#include <doctest/doctest.h>
#include <limits>
#include <sstream>
#include <iostream>
#include <vector>

namespace {

class Grid
{
public:
    static const int X0 = 1;
    static const int Y0 = 1;
    static const int W = 300;

    Grid(int serial)
        : _serial(serial)
        , _levels(W*W)
    {
        for (int i = 0; i < W; ++i)
        {
            for (int j = 0; j < W; ++j)
            {
                _levels[_GetIdx(i,j)] = CalcLevel(i + X0, j + Y0, _serial);
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

        for (int s = 1; s <= W; ++s)
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
        MaxPower ret{};
        for (int i = 0, I = W - size + 1; i < I; ++i)
        {
            for (int j = 0, J = W - size + 1; j < J; ++j)
            {
                auto power = _CalcSquare(X0 + i, Y0 + j, size);
                if (power > ret.power)
                {
                    ret.x = X0 + i;
                    ret.y = Y0 + j;
                    ret.power = power;
                }

            }
        }
        return ret;
    }

    int _CalcSquare(int x, int y, int s) const
    {
        int power{0};

        for (int i = 0; i < s; ++i)
        {
            for (int j = 0; j < s; ++j)
            {
                power += _levels[_GetIdx(x - X0 + i, y - Y0 + j)];
            }
        }

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
