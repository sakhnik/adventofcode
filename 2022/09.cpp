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

size_t TrackTail(int length, std::istream &&is)
{
    std::unordered_set<Point, PointHash> tail_positions;

    std::vector<Point> rope(length);
    tail_positions.insert(rope.back());

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
            rope.front().x += d.x;
            rope.front().y += d.y;

            for (size_t i = 1; i < rope.size(); ++i)
            {
                auto &head = rope[i - 1];
                auto &tail = rope[i];
                auto dx = head.x - tail.x;
                auto dy = head.y - tail.y;
                if (std::abs(dx) > 1 || std::abs(dy) > 1)
                {
                    if (dx)
                        tail.x += dx / std::abs(dx);
                    if (dy)
                        tail.y += dy / std::abs(dy);
                }
                else
                    break;
            }
            tail_positions.insert(rope.back());
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

const char *const TEST2 =
    "R 5\n"
    "U 8\n"
    "L 8\n"
    "D 3\n"
    "R 17\n"
    "D 10\n"
    "L 25\n"
    "U 20\n";

using namespace boost::ut;

suite s = [] {
    "2022-09"_test = [] {
        expect(13_u == TrackTail(2, std::istringstream{TEST}));
        expect(1_u == TrackTail(10, std::istringstream{TEST}));
        expect(36_u == TrackTail(10, std::istringstream{TEST2}));

        Printer::Print(__FILE__, "1", TrackTail(2, std::ifstream{INPUT}));
        Printer::Print(__FILE__, "2", TrackTail(10, std::ifstream{INPUT}));
    };
};

} //namespace;
