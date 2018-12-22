#include <doctest/doctest.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <queue>

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
                sum += _GetRisk(x, y);
            }
        }

        return sum;
    }

    int FindPath()
    {
        struct Way : _Pos
        {
            Way(int x, int y, int tool)
                : _Pos{x, y}, tool{tool}
            {}
            int tool;
            bool operator==(const Way &o) const
            {
                return _Pos::operator==(o) && tool == o.tool;
            }
        };
        Way target{_targetX, _targetY, 1};

        auto wayHash = [](const Way &w) {
            return std::hash<size_t>()(((size_t)w.tool << 30) | ((size_t)w.x << 15) | w.y);
        };
        std::unordered_map<Way, int, decltype(wayHash)> dist(0, wayHash);

        struct Alternative : Way
        {
            Alternative(int x, int y, int tool, int time)
                : Way{x, y, tool}, time{time}
            {}
            int time;
            bool operator<(const Alternative &o) const { return time > o.time; }
        };
        std::priority_queue<Alternative> pq;

        pq.push({0,0,1,0});
        dist[{0,0,1}] = 0;

        auto tryAlternative = [&](const Alternative &a) {
            auto it = dist.find(a);
            if (it != end(dist) && it->second <= a.time)
            {
                return false;
            }
            dist[a] = a.time;
            pq.push(a);
            return true;
        };

        while (!pq.empty())
        {
            Alternative a = pq.top();
            pq.pop();

            if (target == a)
            {
                return a.time;
            }

            // Try to change the tool
            for (int t = 0; t < 3; ++t)
            {
                // 0: rocky - neither
                // 1: wet - torch
                // 2: narrow - climb
                if (t != a.tool && t != _GetRisk(a.x, a.y))
                {
                    tryAlternative({a.x, a.y, t, a.time + 7});
                }
            }

            auto tryMove = [&](int dx, int dy) {
                auto x = a.x + dx;
                auto y = a.y + dy;
                if (x < 0 || y < 0)
                    return;
                if (x > _targetX + 100 || y > _targetY + 100)
                    return;
                if (_GetRisk(x, y) == a.tool)
                    return;
                tryAlternative({x, y, a.tool, a.time + 1});
            };

            tryMove(-1, 0);
            tryMove(1, 0);
            tryMove(0, -1);
            tryMove(0, 1);
        }

        return -1;
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
        return std::hash<IntT>()((static_cast<IntT>(p.x) << (4*sizeof(IntT))) | static_cast<IntT>(p.y));
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

    int _GetRisk(int x, int y)
    {
        return _GetLevel(x, y) % 3;
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        Cave c{510, 10, 10};
        REQUIRE(114 == c.CalcRisk());
        REQUIRE(45 == c.FindPath());
    }

    SUBCASE("task") {
        Cave c{10689, 11, 722};
        MESSAGE(c.CalcRisk());
        MESSAGE(c.FindPath());
    }
}
