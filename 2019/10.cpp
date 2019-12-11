#include <doctest/doctest.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <cmath>


const double EPS = 0.00001;

using MapT = std::vector<std::string>;
using PosT = std::pair<int,int>;

namespace {

MapT GetInput(std::istream &&is)
{
    MapT map;
    std::string line;
    while (getline(is, line))
    {
        if (line.empty())
        {
            continue;
        }
        map.emplace_back(std::move(line));
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

Bearing CalcBearing(int x0, int y0, int x, int y)
{
    int dx = x - x0;
    int dy = y - y0;
    double angle = std::atan2(dx, -dy) / M_PI * 180.0;
    if (angle < 0)
    {
        angle += 360.0;
    }
    return {angle};
}

int CountDirect(int row0, int col0, const MapT &map)
{

    std::map<Bearing, PosT> bearings;

    int count{};
    for (int row = 0; row < map.size(); ++row)
    {
        for (int col = 0; col < map.front().size(); ++col)
        {
            if (map[row][col] != '#')
            {
                continue;
            }

            if (col == col0 && row == row0)
            {
                continue;
            }

            Bearing b = CalcBearing(col0, row0, col, row);
            auto [it, inserted] = bearings.insert({b, PosT{col, row}});
            count += inserted;
        }
    }

    return count;
}

int FindLocation(const MapT &map, PosT &pos)
{
    int max_count = 0;
    for (int row = 0; row < map.size(); ++row)
    {
        for (int col = 0; col < map.front().size(); ++col)
        {
            if (map[row][col] != '#')
            {
                continue;
            }
            int count = CountDirect(row, col, map);
            if (count > max_count)
            {
                max_count = count;
                pos = {col, row};
            }
        }
    }
    return max_count;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    {
        auto b = CalcBearing(0, 0, 0, -1);
        REQUIRE(b == Bearing{0.});
        b = CalcBearing(0, 0, 1, -1);
        REQUIRE(b == Bearing{45});
        b = CalcBearing(0, 0, 1, 0);
        REQUIRE(b == Bearing{90});
        b = CalcBearing(0, 0, 1, 1);
        REQUIRE(b == Bearing{135});
        b = CalcBearing(0, 0, 0, 1);
        REQUIRE(b == Bearing{180});
        b = CalcBearing(0, 0, -1, 1);
        REQUIRE(b == Bearing{225});
        b = CalcBearing(0, 0, -1, 0);
        REQUIRE(b == Bearing{270});
        b = CalcBearing(0, 0, -1, -1);
        REQUIRE(b == Bearing{315});
    }

    {
        const char *const s = R"(
.#..#
.....
#####
....#
...##)";
        auto map = GetInput(std::istringstream{s});
        PosT pos;
        REQUIRE(8 == FindLocation(map, pos));
        REQUIRE(pos.first == 3);
        REQUIRE(pos.second == 4);
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
        PosT pos;
        REQUIRE(33 == FindLocation(map, pos));
        REQUIRE(pos.first == 5);
        REQUIRE(pos.second == 8);
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
        PosT pos;
        REQUIRE(35 == FindLocation(map, pos));
        REQUIRE(pos.first == 1);
        REQUIRE(pos.second == 2);
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
        PosT pos;
        REQUIRE(41 == FindLocation(map, pos));
        REQUIRE(pos.first == 6);
        REQUIRE(pos.second == 3);
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
        PosT pos;
        REQUIRE(210 == FindLocation(map, pos));
        REQUIRE(pos.first == 11);
        REQUIRE(pos.second == 13);
    }

    {
        // task 1
        auto map = GetInput(std::ifstream{INPUT});
        PosT pos;
        MESSAGE(FindLocation(map, pos));
    }
}
