#include <sstream>
#include <fstream>
#include <numeric>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/ut.hpp>

namespace {

struct Point
{
    int x, y;
    bool operator==(const Point &o) const { return x == o.x && y == o.y; }
};

using PointsT = std::vector<Point>;

struct Box
{
    Point min, max;
    bool operator==(const Box &o) const { return min == o.min && max == o.max; }
};

PointsT GetInput(std::istream &&is)
{
    PointsT points;
    Point p;
    char comma;
    while (is && (is >> p.x >> comma >> p.y))
    {
        points.push_back(p);
    }
    return points;
}

// Determine the bounding box to work with the smallest field possible
Box GetBox(const PointsT &points)
{
    auto MIN_V = std::numeric_limits<int>::min();
    auto MAX_V = std::numeric_limits<int>::max();
    Box box = {
        {MAX_V, MAX_V},
        {MIN_V, MIN_V}
    };

    for (const auto &p : points)
    {
        if (p.x < box.min.x) box.min.x = p.x;
        if (p.y < box.min.y) box.min.y = p.y;
        if (p.x > box.max.x) box.max.x = p.x;
        if (p.y > box.max.y) box.max.y = p.y;
    }

    return box;
}

class Field
{
public:
    Field(const PointsT &points)
        : _box{GetBox(points)}
        , _width{_box.max.x - _box.min.x + 1}
        , _height{_box.max.y - _box.min.y + 1}
        , _field(_width*_height, -1)
    {
    }

    int GetHeight() const { return _height; }
    int GetWidth() const { return _width; }

    std::vector<int>& Get() { return _field; }

    int& GetXY(int x, int y)
    {
        return _field[y * _width + x];
    }

    // Count the Manhattan distance from a given position in the field
    // to a point.
    int Distance(int x, int y, const Point &p)
    {
        return std::abs(p.x - (x + _box.min.x)) + std::abs(p.y - (y + _box.min.y));
    }

private:
    Box _box;
    int _width, _height;
    std::vector<int> _field;
};

// We'll be iterating over every position in the bounding box,
// and determine the prevailing point for every position.
int FindSpot(const PointsT &points)
{
    Field field(points);

    // Remember distances together with points.
    struct Dist
    {
        int distance;
        int pointNum;
        bool operator<(const Dist &o) const { return distance < o.distance; }
    };
    std::vector<Dist> distances(points.size());

    // For every position in the field.
    for (int y = 0; y < field.GetHeight(); ++y)
    {
        for (int x = 0; x < field.GetWidth(); ++x)
        {
            // Calculate distances to every point.
            for (size_t i = 0; i < points.size(); ++i)
            {
                distances[i].distance = field.Distance(x, y, points[i]);
                distances[i].pointNum = i;
            }

            // Order the first two smallest distances
            std::nth_element(begin(distances), begin(distances) + 1, end(distances));

            // If they aren't tied (equal), mark the prevailing point on this position.
            if (distances[0].distance < distances[1].distance)
            {
                field.GetXY(x, y) = distances[0].pointNum;
            }
        }
    }

    // Ignore the ties.
    auto &f = field.Get();
    auto it_end = std::remove(begin(f), end(f), -1);
    // Sort the field to be able to calculate the most frequent point (biggest area).
    std::sort(begin(f), it_end);

    int max_area{0};
    //int point = -1;
    for (auto it = begin(f); it != it_end; )
    {
        auto it2 = std::find_if_not(it, it_end,
                                    [pnum=*it](auto i) { return i == pnum; });
        auto area = it2 - it;
        if (area > max_area)
        {
            max_area = area;
            //point = *it;
        }
        it = it2;
    }
    
    return max_area;
}

int FindAreaWithin(const PointsT &points, int distance)
{
    Field field(points);

    int count{0};

    // For every position in the field.
    for (int y = 0; y < field.GetHeight(); ++y)
    {
        for (int x = 0; x < field.GetWidth(); ++x)
        {
            // Calculate the sum of distances to every point.
            auto d = std::accumulate(begin(points), end(points), 0,
                            [&](int d, const auto &p) {
                                return d + field.Distance(x, y, p);
                            });
            if (d < distance)
            {
                ++count;
            }
        }
    }

    return count;
}

using namespace boost::ut;

suite s = [] {
    "2018-06"_test = [] {
        auto test = GetInput(std::istringstream("1, 1\n1, 6\n8, 3\n3, 4\n5, 5\n8, 9"));
        expect(6_u == test.size());
        expect(Box{{1, 1}, {8, 9}} == GetBox(test));
        expect(17_i == FindSpot(test));

        auto input = GetInput(std::ifstream(INPUT));
        std::cout << "2018-06.1: " << FindSpot(input) << std::endl;

        expect(16_i == FindAreaWithin(test, 32));
        std::cout << "2018-06.2: " << FindAreaWithin(input, 10000) << std::endl;
    };
};

} //namespace;