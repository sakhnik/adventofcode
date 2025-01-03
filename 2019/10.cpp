#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <algorithm>
#include <list>

#include "../test.hpp"

namespace {

const double EPS = 0.00001;

struct Pos
{
    int x, y;

    bool operator==(const Pos &o) const
    {
        return x == o.x && y == o.y;
    }
};

using MapT = std::vector<Pos>;

MapT GetInput(std::istream &&is)
{
    MapT map;
    std::string line;
    int y{0};
    while (getline(is, line))
    {
        if (line.empty())
        {
            continue;
        }
        for (int x = 0; x < static_cast<int>(line.size()); ++x)
        {
            if (line[x] == '#')
            {
                map.push_back({x, y});
            }
        }
        ++y;
    }
    return map;
}

struct Bearing
{
    double v;

    bool operator==(Bearing o) const
    {
        return std::abs(v - o.v) < EPS;
    }

    bool operator<(const Bearing &o) const
    {
        return std::abs(v - o.v) > EPS && v < o.v;
    }
};

std::ostream& operator<<(std::ostream &os, Bearing b)
{
    return os << b.v;
}

Bearing CalcBearing(Pos p0, Pos p)
{
    int dx = p.x - p0.x;
    int dy = p.y - p0.y;
    double angle = std::atan2(dx, -dy) / M_PI * 180.0;
    if (angle < 0)
    {
        angle += 360.0;
    }
    return {angle};
}

int CountDirect(Pos orig, const MapT &map)
{

    std::map<Bearing, Pos> bearings;

    int count{};
    for (auto a : map)
    {
        if (a == orig)
        {
            continue;
        }

        Bearing b = CalcBearing(orig, a);
        auto [it, inserted] = bearings.insert({b, a});
        count += inserted;
    }

    return count;
}

int FindLocation(const MapT &map, Pos &pos)
{
    int max_count = 0;
    for (auto a : map)
    {
        int count = CountDirect(a, map);
        if (count > max_count)
        {
            max_count = count;
            pos = a;
        }
    }
    return max_count;
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "10"_test = [] {
        {
            auto b = CalcBearing({0, 0}, {0, -1});
            expect(eq(b, Bearing{0.}));
            b = CalcBearing({0, 0}, {1, -1});
            expect(eq(b, Bearing{45}));
            b = CalcBearing({0, 0}, {1, 0});
            expect(eq(b, Bearing{90}));
            b = CalcBearing({0, 0}, {1, 1});
            expect(eq(b, Bearing{135}));
            b = CalcBearing({0, 0}, {0, 1});
            expect(eq(b, Bearing{180}));
            b = CalcBearing({0, 0}, {-1, 1});
            expect(eq(b, Bearing{225}));
            b = CalcBearing({0, 0}, {-1, 0});
            expect(eq(b, Bearing{270}));
            b = CalcBearing({0, 0}, {-1, -1});
            expect(eq(b, Bearing{315}));
        }

        {
            const char *const s = R"(
.#..#
.....
#####
....#
...##)";
            auto map = GetInput(std::istringstream{s});
            Pos pos;
            expect(8_i == FindLocation(map, pos));
            expect(3_i == pos.x);
            expect(4_i == pos.y);
        }

        {
            const char *const s = R"(
......#.#.
#..#.#....
..#######.
.#.#.###..
.#..#.....
..#....#.#
#..#....#.
.##.#..###
##...#..#.
.#....####)";
            auto map = GetInput(std::istringstream{s});
            Pos pos;
            expect(33_i == FindLocation(map, pos));
            expect(5_i == pos.x);
            expect(8_i == pos.y);
        }

        {
            const char *const s = R"(
#.#...#.#.
.###....#.
.#....#...
##.#.#.#.#
....#.#.#.
.##..###.#
..#...##..
..##....##
......#...
.####.###.)";
            auto map = GetInput(std::istringstream{s});
            Pos pos;
            expect(35_i == FindLocation(map, pos));
            expect(1_i == pos.x);
            expect(2_i == pos.y);
        }

        {
            const char *const s = R"(
.#..#..###
####.###.#
....###.#.
..###.##.#
##.##.#.#.
....###..#
..#.#..#.#
#..#.#.###
.##...##.#
.....#.#..)";
            auto map = GetInput(std::istringstream{s});
            Pos pos;
            expect(41_i == FindLocation(map, pos));
            expect(6_i == pos.x);
            expect(3_i == pos.y);
        }

        {
            const char *const s = R"(
.#..##.###...#######
##.############..##.
.#.######.########.#
.###.#######.####.#.
#####.##.#.##.###.##
..#####..#.#########
####################
#.####....###.#.#.##
##.#################
#####.##.###..####..
..######..##.#######
####.##.####...##..#
.#####..#.######.###
##...#.##########...
#.##########.#######
.####.#.###.###.#.##
....##.##.###..#####
.#.#.###########.###
#.#.#.#####.####.###
###.##.####.##.#..##)";
            auto map = GetInput(std::istringstream{s});
            Pos pos;
            expect(210_i == FindLocation(map, pos));
            expect(11_i == pos.x);
            expect(13_i == pos.y);
        }

        // task 1
        auto map = GetInput(std::ifstream{INPUT});
        Pos pos;
        Printer::Print(__FILE__, "1", FindLocation(map, pos));

        auto calcD = [](Pos a, Pos b) {
            auto dx = b.x - a.x;
            auto dy = b.y - a.y;
            return dx * dx + dy * dy;
        };

        struct Info
        {
            Pos pos;
            Bearing b;
            int dist;
        };

        std::list<Info> queue;
        for (Pos p : map)
        {
            if (p == pos)
            {
                continue;
            }
            queue.push_back({p, CalcBearing(pos, p), calcD(pos, p)});
        }

        queue.sort(
            [&](Info &a, Info &b) {
                if (a.b < b.b)
                {
                    return true;
                }
                if (a.b == b.b)
                {
                    return a.dist < b.dist;
                }
                return false;
            });

        int count{200};
        while (count > 0)
        {
            Bearing last_b{-1};
            for (auto it = queue.begin(); it != queue.end();)
            {
                auto b = it->b;
                if (b == last_b)
                {
                    // skip
                    ++it;
                }
                else
                {
                    if (--count <= 0)
                    {
                        Printer::Print(__FILE__, "2", it->pos.x * 100 + it->pos.y);
                        break;
                    }

                    it = queue.erase(it);
                    last_b = b;
                }
            }
        }
    };
};

} //namespace;
