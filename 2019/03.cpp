#include <doctest/doctest.h>
#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <fstream>


enum Direction
{
    HORIZONTAL = 0,
    VERTICAL
};

struct Segment
{
    Direction dir;
    int value;  // height, for instance
    int start;
    int end;    // start <= end
};

int GetIntersection(const Segment &a, const Segment &b)
{
    if (a.dir != b.dir)
    {
        assert(a.start <= a.end);
        assert(b.start <= b.end);
        if ((a.value >= b.start && a.value <= b.end) &&
            (b.value >= a.start && b.value <= a.end))
        {
            return std::abs(a.value) + std::abs(b.value);
        }
        return std::numeric_limits<int>::max();
    }

    // The lines are parallel
    if (a.value != b.value)
    {
        return std::numeric_limits<int>::max();
    }

    // TODO: colinear
    //auto start = std::max(a.start, b.start);
    //auto end = std::min(a.end, b.end);
    return std::numeric_limits<int>::max();
}

using PathT = std::vector<Segment>;

PathT ParseInput(const std::string &input)
{
    PathT path;

    int x{}, y{};
    char direction{}, comma{};
    int length{};
    std::istringstream iss{input};

    while (iss)
    {
        iss >> direction >> length >> comma;
        switch (direction)
        {
        case 'R':
            path.push_back({HORIZONTAL, y, x, x + length});
            x += length;
            break;
        case 'D':
            path.push_back({VERTICAL, x, y - length, y});
            y -= length;
            break;
        case 'L':
            path.push_back({HORIZONTAL, y, x - length, x});
            x -= length;
            break;
        case 'U':
            path.push_back({VERTICAL, x, y, y + length});
            y += length;
            break;
        }
    }

    return path;
}

int FindClosestIntersection(const PathT &p1, const PathT &p2)
{
    int dist{std::numeric_limits<int>::max()};
    for (const auto &a : p1)
    {
        for (const auto &b : p2)
        {
            auto d = GetIntersection(a, b);
            if (d && d < dist)
            {
                dist = d;
            }
        }
    }
    return dist;
}

TEST_CASE(TEST_NAME)
{
    REQUIRE(6 == FindClosestIntersection(ParseInput("R8,U5,L5,D3"), ParseInput("U7,R6,D4,L4")));
    REQUIRE(159 == FindClosestIntersection(ParseInput("R75,D30,R83,U83,L12,D49,R71,U7,L72"),
                                           ParseInput("U62,R66,U55,R34,D71,R55,D58,R83")));
    REQUIRE(135 == FindClosestIntersection(ParseInput("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51"),
                                           ParseInput("U98,R91,D20,R16,D67,R40,U7,R15,U6,R7")));

    std::ifstream ifs(INPUT);
    std::string s;
    std::getline(ifs, s);
    auto p1 = ParseInput(s);
    std::getline(ifs, s);
    auto p2 = ParseInput(s);
    MESSAGE(FindClosestIntersection(p1, p2));
}
