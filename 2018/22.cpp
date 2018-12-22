#include <doctest/doctest.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <unordered_map>

namespace {

class Cave
{
public:
    using IntT = int64_t;
    Cave(IntT depth, int targetX, int targetY)
        : _depth{depth}
        , _targetX{targetX}
        , _targetY{targetY}
    {
    }

    IntT CalcRisk()
    {
        IntT sum{};

        for (int y = 0; y <= _targetY; ++y)
        {
            for (int x = 0; x <= _targetX; ++x)
            {
                sum += _GetLevel(x, y) % 3;
            }
        }

        return sum;
    }

private:
    IntT _depth;
    int _targetX, _targetY;
    struct _Pos
    {
        int x, y;
        bool operator==(const _Pos &o) const { return x == o.x && y == o.y; }
    };
    static constexpr auto _posHash = [](const _Pos &p) {
        return std::hash<IntT>()((static_cast<IntT>(p.x) << (4*sizeof(IntT))) + static_cast<IntT>(p.y));
    };
    std::unordered_map<_Pos, IntT, decltype(_posHash)> _levels{0, _posHash};

    IntT _CalcIndex(int x, int y)
    {
        if (x == 0 && y == 0)
            return 0;
        if (x == _targetX && y == _targetY)
            return 0;
        if (y == 0)
            return x * 16807;
        if (x == 0)
            return y * 48271;
        return _GetLevel(x - 1, y) * _GetLevel(x, y - 1);
    }

    IntT _GetLevel(int x, int y)
    {
        auto it = _levels.find({x, y});
        if (it != _levels.end())
            return it->second;
        auto level = (_CalcIndex(x, y) + _depth) % 20183;
        _levels.insert({{x, y}, level});
        return level;
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
