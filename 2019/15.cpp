#include "IntCode.h"
#include <fstream>
#include <map>
#include <queue>
#include "../test.hpp"

namespace {

// Ordered position on the map
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

using namespace boost::ut;

suite s = [] {
    "2019-15"_test = [] {
        std::ifstream ifs{INPUT};
        IntCode prog{ifs};

        // Let's explore the map first
        Pos robot{}, target{};
        std::map<Pos, char> map{{robot, '.'}};

        auto printMap = [&]() {
            Pos tl{}, br{};
            for (const auto &[p, c] : map)
            {
                if (p.x < tl.x)
                    tl.x = p.x;
                if (p.y < tl.y)
                    tl.y = p.y;
                if (p.x > br.x)
                    br.x = p.x;
                if (p.y > br.y)
                    br.y = p.y;
            }

            for (int y = tl.y; y <= br.y; ++y)
            {
                for (int x = tl.x; x <= br.x; ++x)
                {
                    auto it = map.find({x, y});
                    if (it == map.end())
                    {
                        std::cout << ' ';
                        continue;
                    }
                    if (!x && !y)
                        std::cout << 'X';
                    else if (target == Pos{x, y})
                    {
                        std::cout << '*';
                    }
                    else
                        std::cout << it->second;
                }
                std::cout << "\n";
            }
        };

        // Calculate the next position of the robot in the given direction.
        auto nextPos = [&](int direction) {
            Pos ret{robot};
            switch (direction)
            {
            case 1: --ret.y;
                break;
            case 2: ++ret.y;
                break;
            case 3: --ret.x;
                break;
            case 4: ++ret.x;
                break;
            }
            return ret;
        };

        // The input to the firmware
        int input{1};
        // Exploration track node
        struct Track
        {
            Pos pos;           // position
            int direction = 0; // yet non-explored directions (1..4)
            int from = 0;      // from which direction we came here
        };
        std::vector<Track> track{{robot}}; // backtracking path

        // Step into the unknown (given direction)
        auto createNextTrack = [&](int direction) {
            Track t;
            t.pos = nextPos(direction);
            const int opposites[] = {2, 1, 4, 3};
            t.from = opposites[direction - 1];
            return t;
        };

        // Exploration process
        auto explore = [&]() {
            while (!track.empty())
            {
                auto &t = track.back();

                // If probed all possible directions, backtrack.
                if (t.direction >= 4)
                {
                    // backtrack
                    input = t.from;
                    track.pop_back();
                    return;
                }

                // Try the next direction
                input = ++t.direction;
                auto robotNext = createNextTrack(input);
                if (!map.count(robotNext.pos))
                {
                    track.push_back(robotNext);
                    return;
                }
            }
            // If nowhere to backtrack, end the program.
            input = 0;
        };

        // Start exploring
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
                    // No way, the wall
                    const auto &t = track.back();
                    map.emplace(t.pos, '#');
                    track.pop_back(); // dead end
                }
                else if (r == 1)
                {
                    // Passage
                    const auto &t = track.back();
                    map.emplace(t.pos, '.');
                    robot = t.pos;
                }
                else if (r == 2)
                {
                    // Oxygen target found
                    const auto &t = track.back();
                    map.emplace(t.pos, '.');
                    target = robot = t.pos;
                }
                explore();
            }
        }

        printMap();

        // Run BFS from the given position, return the map of distances.
        auto bfs = [&](Pos start) {
            std::map<Pos, int> distance{{start, 0}};
            std::queue<Pos> todo;
            todo.push(start);

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

            return distance;
        };

        {
            // Task 1: the distance to the oxygen.
            auto distance = bfs({0, 0});
            Printer::Print(__FILE__, "1", distance.at(target));
        }

        {
            // Task 2: the farthest distance from the oxygen.
            auto distance = bfs(target);
            auto it = std::max_element(begin(distance), end(distance),
                                       [](const auto &a, const auto &b) { return a.second < b.second; });
            Printer::Print(__FILE__, "2", it->second);
        }
    };
};

} //namespace;