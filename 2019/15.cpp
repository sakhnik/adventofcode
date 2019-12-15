#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>
#include <map>
#include <queue>


namespace {

struct Pos
{
    int x,y;

    bool operator<(const Pos &o) const
    {
        if (x < o.x) return true;
        if (x > o.x) return false;
        return y < o.y;
    }

    bool operator==(const Pos &o) const
    {
        return x == o.x && y == o.y;
    }
};

} //namespace;


TEST_CASE(TEST_NAME)
{
    srand((unsigned)time(0));

    std::ifstream ifs{INPUT};
    IntCode prog{ifs};

    Pos robot{}, target{};
    std::map<Pos, char> map{{robot, '.'}};

    auto printMap = [&]() {
        Pos tl{}, br{};
        for (const auto &[p, c] : map)
        {
            if (p.x < tl.x) tl.x = p.x;
            if (p.y < tl.y) tl.y = p.y;
            if (p.x > br.x) br.x = p.x;
            if (p.y > br.y) br.y = p.y;
        }

        for (int y = tl.y; y <= br.y; ++y)
        {
            for (int x = tl.x; x <= br.x; ++x)
            {
                auto it = map.find({x,y});
                if (it == map.end())
                {
                    std::cout << ' ';
                    continue;
                }
                if (!x && !y)
                    std::cout << 'X';
                else if (target == Pos{x,y})
                {
                    std::cout << '*';
                }
                else
                    std::cout << it->second;
            }
            std::cout << "\n";
        }
    };

    auto nextPos = [&](int direction) {
        Pos ret{robot};
        switch (direction)
        {
        case 1: --ret.y; break;
        case 2: ++ret.y; break;
        case 3: --ret.x; break;
        case 4: ++ret.x; break;
        }
        return ret;
    };

    int input{1};
    struct Track
    {
        Pos pos;
        int direction = 0;
        int from = 0;
    };
    std::vector<Track> track{{robot}};

    auto createNextTrack = [&](int direction) {
        Track t;
        t.pos = nextPos(direction);
        const int opposites[] = {2, 1, 4, 3};
        t.from = opposites[direction - 1];
        return t;
    };

    auto explore = [&]() {
        while (!track.empty())
        {
            auto &t = track.back();

            if (t.direction >= 4)
            {
                // backtrack
                input = t.from;
                track.pop_back();
                return;
            }

            input = ++t.direction;
            auto robotNext = createNextTrack(input);
            if (!map.count(robotNext.pos))
            {
                track.push_back(robotNext);
                return;
            }
        }
        input = 0;
    };

    explore();

    while (prog.GetState() != prog.S_HALT)
    {
        auto r = prog.Advance(input);

        switch (prog.GetState())
        {
        case prog.S_HALT:
        case prog.S_RUN:
        case prog.S_INPUT:
            continue;
        case prog.S_OUTPUT:
            if (r == 0)
            {
                const auto &t = track.back();
                map.emplace(t.pos, '#');
                track.pop_back();  // dead end
            }
            else if (r == 1)
            {
                const auto &t = track.back();
                map.emplace(t.pos, '.');
                robot = t.pos;
            }
            else if (r == 2)
            {
                const auto &t = track.back();
                map.emplace(t.pos, '.');
                target = robot = t.pos;
            }
            explore();
        }
    }

    printMap();

    auto bfs = [&]() {
        std::map<Pos, int> distance{{{0,0}, 0}};
        std::queue<Pos> todo;
        todo.push({0,0});

        while (!todo.empty())
        {
            auto pos = todo.front();
            todo.pop();
            auto curDist = distance.at(pos);

            auto probe = [&](Pos next) {
                if (map.at(next) == '#')
                    return;
                auto it = distance.find(next);
                if (it == distance.end())
                {
                    distance.emplace(next, curDist + 1);
                    todo.push(next);
                }
            };
            probe({pos.x, pos.y - 1});
            probe({pos.x, pos.y + 1});
            probe({pos.x - 1, pos.y});
            probe({pos.x + 1, pos.y});
        }

        return distance.at(target);
    };

    MESSAGE(bfs());
}
