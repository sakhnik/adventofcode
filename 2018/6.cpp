#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <sstream>
#include <fstream>

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

// We'll be iterating over every position in the bounding box,
// and determine the prevailing point for every position.
int FindSpot(const PointsT &points)
{
    // Determine the bounding box.
    auto box = GetBox(points);
    auto width = box.max.x - box.min.x + 1;
    auto height = box.max.y - box.min.y + 1;
    std::vector<int> field(width*height, -1);

    // Easy access the field.
    auto mark = [&](int x, int y) -> int& {
        return field[y * width + x];
    };

    // Count the Manhattan distance from a given position in the field
    // to a point.
    auto distance = [&](int x, int y, const Point &p) {
        return std::abs(p.x - (x + box.min.x)) + std::abs(p.y - (y + box.min.y));
    };

    // Remember distances together with points.
    struct Dist
    {
        int distance;
        int pointNum;
        bool operator<(const Dist &o) const { return distance < o.distance; }
    };
    std::vector<Dist> distances(points.size());

    // For every position in the field.
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // Calculate distances to every point.
            for (size_t i = 0; i < points.size(); ++i)
            {
                distances[i].distance = distance(x, y, points[i]);
                distances[i].pointNum = i;
            }

            // Order the first two smallest distances
            std::nth_element(begin(distances), begin(distances) + 1, end(distances));

            // If they aren't tied (equal), mark the prevailing point on this position.
            if (distances[0].distance < distances[1].distance)
            {
                mark(x, y) = distances[0].pointNum;
            }
        }
    }

    // Ignore the ties.
    auto it_end = std::remove(begin(field), end(field), -1);
    // Sort the field to be able to calculate the most frequent point (biggest area).
    std::sort(begin(field), it_end);

    int max_area{0};
    int point = -1;
    for (auto it = begin(field); it != it_end; )
    {
        auto it2 = std::find_if_not(it, it_end,
                                    [pnum=*it](auto i) { return i == pnum; });
        auto area = it2 - it;
        if (area > max_area)
        {
            max_area = area;
            point = *it;
        }
        it = it2;
    }
    
    return max_area;
}

TEST_CASE("main")
{
    auto test = GetInput(std::istringstream("1, 1\n1, 6\n8, 3\n3, 4\n5, 5\n8, 9"));
    REQUIRE(6 == test.size());
    REQUIRE(Box{{1,1}, {8,9}} == GetBox(test));
    REQUIRE(17 == FindSpot(test));

    auto input = GetInput(std::ifstream(INPUT));
    std::cout << FindSpot(input) << std::endl;
}
