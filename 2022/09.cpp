#include "../test.hpp"
#include <fstream>
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

size_t TrackTail(std::istream &&is)
{
    std::unordered_set<Point, PointHash> tail_positions;

    Point head{0, 0};
    Point tail{0, 0};
    tail_positions.insert(tail);

    char dir{};
    int count{};
    while (is >> dir >> count)
    {
        Point d{};
        switch (dir)
        {
        case 'L': d.x = -1; break;
        case 'R': d.x = 1; break;
        case 'U': d.y = -1; break;
        case 'D': d.y = 1; break;
        }
        while (count--)
        {
            head.x += d.x;
            head.y += d.y;

            auto dx = head.x - tail.x;
            auto dy = head.y - tail.y;
            if (std::abs(dx) > 1 || std::abs(dy) > 1)
            {
                if (dx)
                    tail.x += dx / std::abs(dx);
                if (dy)
                    tail.y += dy / std::abs(dy);

                tail_positions.insert(tail);
            }
        }
    }
    return tail_positions.size();
}

const char *const TEST =
    "R 4\n"
    "U 4\n"
    "L 3\n"
    "D 1\n"
    "R 4\n"
    "D 1\n"
    "L 5\n"
    "R 2\n";

using namespace boost::ut;

suite s = [] {
    "2022-09"_test = [] {
        expect(13_u == TrackTail(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", TrackTail(std::ifstream{INPUT}));
    };
};

} //namespace;
