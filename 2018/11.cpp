#include <doctest/doctest.h>
#include <limits>

namespace {

class Grid
{
public:
    struct Vec
    {
        int x, y;
        bool operator==(const Vec &o) const { return x == o.x && y == o.y; }
    };

    Grid(Vec tl, Vec br, int serial)
        : _tl(tl)
        , _wh{br.x - tl.x + 1, br.y - tl.y + 1}
        , _serial(serial)
    {
    }

    Vec FindMaxPower() const
    {
        Vec ret{};
        int max_power = std::numeric_limits<int>::min();
        for (int i = 0, I = _wh.x - 2; i < I; ++i)
        {
            for (int j = 0, J = _wh.y - 2; j < J; ++j)
            {
                auto power = _Calc3x3(_tl.x + i, _tl.y + j);
                if (power > max_power)
                {
                    max_power = power;
                    ret = Vec{_tl.x + i, _tl.y + j};
                }

            }
        }
        return ret;
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
    Vec _tl;
    Vec _wh;
    int _serial;

    int _Calc3x3(int x, int y) const
    {
        int power{0};

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                power += CalcLevel(x + i, y + j, _serial);
            }
        }

        return power;
    }
};

class StGrid
    : public Grid
{
public:
    StGrid(int serial)
        : Grid{{1,1}, {300,300}, serial}
    {
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
        REQUIRE(Grid::Vec{33,45} == StGrid{18}.FindMaxPower());
        REQUIRE(Grid::Vec{21,61} == StGrid{42}.FindMaxPower());
    }

    SUBCASE("task1") {
        auto v = StGrid{1723}.FindMaxPower();
        MESSAGE(v.x << "," << v.y);
    }
}
