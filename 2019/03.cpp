#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <fstream>
#include <boost/ut.hpp>

#include "../Printer.hpp"

namespace {

struct Segment
{
    char direction;
    int value;  // height, for instance
    int start;
    int end;
    int distance;

    bool isHorizontal() const
    {
        return direction == 'L' || direction == 'R';
    }
};

int GetIntersection(const Segment &a, const Segment &b)
{
    if (a.isHorizontal() != b.isHorizontal())
    {
        double r1 = 1. * (a.value - b.start) / (b.end - b.start);
        double r2 = 1. * (b.value - a.start) / (a.end - a.start);
        if ((r1 >= 0. && r1 <= 1.) && (r2 >= 0. && r2 <= 1.))
        {
            return std::abs(a.value) + std::abs(b.value);
        }
        return std::numeric_limits<int>::max();
    }

    //// The lines are parallel
    //if (a.value != b.value)
    //{
    //    return std::numeric_limits<int>::max();
    //}

    // TODO: colinear
    //auto start = std::max(a.start, b.start);
    //auto end = std::min(a.end, b.end);
    return std::numeric_limits<int>::max();
}

int GetIntersection2(const Segment &a, const Segment &b)
{
    if (a.isHorizontal() != b.isHorizontal())
    {
        double r1 = 1. * (a.value - b.start) / (b.end - b.start);
        double r2 = 1. * (b.value - a.start) / (a.end - a.start);
        if ((r1 >= 0. && r1 <= 1.) && (r2 >= 0. && r2 <= 1.))
        {
            return a.distance + b.distance
                + static_cast<int>(0.5
                                   + r1 * std::abs(b.end - b.start)
                                   + r2 * std::abs(a.end - a.start));
        }
        return std::numeric_limits<int>::max();
    }

    return std::numeric_limits<int>::max();
}

using PathT = std::vector<Segment>;

PathT ParseInput(const std::string &input)
{
    PathT path;

    int x{}, y{};
    char direction{}, comma{};
    int length{};
    int distance{};
    std::istringstream iss{input};

    while (iss)
    {
        iss >> direction >> length >> comma;
        switch (direction)
        {
        case 'R':
            path.push_back({direction, y, x, x + length, distance});
            x += length;
            break;
        case 'D':
            path.push_back({direction, x, y, y - length, distance});
            y -= length;
            break;
        case 'L':
            path.push_back({direction, y, x, x - length, distance});
            x -= length;
            break;
        case 'U':
            path.push_back({direction, x, y, y + length, distance});
            y += length;
            break;
        }
        distance += length;
    }

    return path;
}

template <typename FuncT>
int FindClosestIntersection(const PathT &p1, const PathT &p2, FuncT func)
{
    int dist{std::numeric_limits<int>::max()};
    for (const auto &a : p1)
    {
        for (const auto &b : p2)
        {
            auto d = func(a, b);
            if (d && d < dist)
            {
                dist = d;
            }
        }
    }
    return dist;
}

using namespace boost::ut;

suite s = [] {
    "2019-03"_test = [] {
        expect(6_i == FindClosestIntersection(ParseInput("R8,U5,L5,D3"),
                                              ParseInput("U7,R6,D4,L4"),
                                              GetIntersection));
        expect(159_i == FindClosestIntersection(ParseInput("R75,D30,R83,U83,L12,D49,R71,U7,L72"),
                                                ParseInput("U62,R66,U55,R34,D71,R55,D58,R83"),
                                                GetIntersection));
        expect(135_i == FindClosestIntersection(ParseInput("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51"),
                                                ParseInput("U98,R91,D20,R16,D67,R40,U7,R15,U6,R7"),
                                                GetIntersection));

        std::ifstream ifs(INPUT);
        std::string s;
        std::getline(ifs, s);
        auto p1 = ParseInput(s);
        std::getline(ifs, s);
        auto p2 = ParseInput(s);
        Printer::Print(__FILE__, "1", FindClosestIntersection(p1, p2, GetIntersection));

        expect(30_i == FindClosestIntersection(ParseInput("R8,U5,L5,D3"),
                                               ParseInput("U7,R6,D4,L4"),
                                               GetIntersection2));
        expect(610_i == FindClosestIntersection(ParseInput("R75,D30,R83,U83,L12,D49,R71,U7,L72"),
                                                ParseInput("U62,R66,U55,R34,D71,R55,D58,R83"),
                                                GetIntersection2));
        expect(410_i == FindClosestIntersection(ParseInput("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51"),
                                                ParseInput("U98,R91,D20,R16,D67,R40,U7,R15,U6,R7"),
                                                GetIntersection2));
        Printer::Print(__FILE__, "2", FindClosestIntersection(p1, p2, GetIntersection2));
    };
};

} //namespace;