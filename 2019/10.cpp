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

int CountDirect(int row0, int col0, const MapT &map)
{
    struct VecT
    {
        double x;
        double y;

        bool operator==(const VecT &o) const
        {
            return std::abs(x - o.x) < EPS && std::abs(y - o.y) < EPS;
        }

        bool operator<(const VecT &o) const
        {
            if (std::abs(x - o.x) > EPS)
            {
                if (x < o.x)
                {
                    return true;
                }
                if (x > o.x)
                {
                    return false;
                }
            }
            if (std::abs(y - o.y) > EPS)
            {
                return y < o.y;
            }
            return false;
        }
    };

    std::map<VecT, PosT> vectors;

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

            int dx = col - col0;
            int dy = row - row0;
            double length = std::sqrt(dx * dx + dy * dy);
            VecT vec{dx / length, dy / length};

            auto [it, inserted] = vectors.insert({vec, PosT{col, row}});
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
