#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

struct Point
{
    int x{}, y{};

    bool operator==(const Point &o) const = default;
};

struct PointHash
{
    size_t operator()(const Point &p) const noexcept
    {
        size_t seed{};
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        return seed;
    }
};

using FieldT = std::unordered_map<Point, int, PointHash>;

struct Vector
{
    Point a{}, b{};
};

int Sign(int d)
{
    if (d < 0) return -1;
    return d > 0;
}

void Trace(Vector v, FieldT &points)
{
    int dx = Sign(v.b.x - v.a.x);
    int dy = Sign(v.b.y - v.a.y);

    if (dx != 0 && dy != 0)
        return;
    Point p{v.a};
    while (p != v.b)
    {
        ++points[p];
        p.x += dx;
        p.y += dy;
    }
    ++points[p];
}

using LinesT = std::vector<Vector>;

LinesT Parse(std::istream &&is)
{
    LinesT lines;
    std::string line;
    while (getline(is, line))
    {
        Vector v;
        if (4 != sscanf(line.c_str(), "%d,%d -> %d,%d", &v.a.x, &v.a.y, &v.b.x, &v.b.y))
            continue;
        lines.push_back(v);
    }
    return lines;
}

size_t CountIntersections(const LinesT &lines)
{
    FieldT field;
    for (const auto &line : lines)
        Trace(line, field);

    return std::count_if(field.begin(), field.end(), [](const auto &en) { return en.second > 1; });

}

using namespace boost::ut;

const char *const TEST_INPUT = R"(
0,9 -> 5,9
8,0 -> 0,8
9,4 -> 3,4
2,2 -> 2,1
7,0 -> 7,4
6,4 -> 2,0
0,9 -> 2,9
3,4 -> 1,4
0,0 -> 8,8
5,5 -> 8,2
)";

suite s = [] {
    "2021-05"_test = [] {
        auto test_lines = Parse(std::istringstream{TEST_INPUT});
        expect(5_u == CountIntersections(test_lines));

        auto lines = Parse(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", CountIntersections(lines));
    };
};

} //namespace;
