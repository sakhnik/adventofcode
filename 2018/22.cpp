#include <doctest/doctest.h>
#include <iostream>
#include <vector>
#include <numeric>

namespace {

class Cave
{
public:
    using IntT = int64_t;
    Cave(IntT depth, size_t targetX, size_t targetY)
        : _width(targetX + 1)
        , _height(targetY + 1)
        , _levels(_width * _height)
    {
        auto calcIndex = [&, targetX, targetY](size_t x, size_t y) -> IntT {
            if (x == 0 && y == 0)
                return 0;
            if (x == targetX && y == targetY)
                return 0;
            if (y == 0)
                return x * 16807;
            if (x == 0)
                return y * 48271;
            return _levels[_Idx(x-1, y)] * _levels[_Idx(x, y-1)];
        };

        auto calcLevel = [&, depth](size_t x, size_t y) {
            return (calcIndex(x, y) + depth) % 20183;
        };

        for (size_t y = 0; y < _height; ++y)
        {
            for (size_t x = 0; x < _width; ++x)
            {
                _levels[_Idx(x, y)] = calcLevel(x, y);
            }
        }
    }

    IntT CalcRisk() const
    {
        return std::accumulate(begin(_levels), end(_levels), 0,
                               [](auto s, auto l) { return s + l % 3; });
    }

private:
    size_t _width, _height;
    std::vector<IntT> _levels;

    size_t _Idx(size_t x, size_t y) const
    {
        return y * _width + x;
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        Cave c{510, 10, 10};
        REQUIRE(114 == c.CalcRisk());
    }

    SUBCASE("task") {
        Cave c{10689, 11, 722};
        MESSAGE(c.CalcRisk());
    }
}
