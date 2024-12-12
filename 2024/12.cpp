#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

using namespace boost::ut;

struct Pos
{
    int x, y;
    bool operator==(const Pos &o) const = default;
    struct Hash
    {
        size_t operator()(const Pos &p) const
        {
            size_t s = p.x;
            boost::hash_combine(s, p.y);
            return s;
        }
    };
};

std::pair<int, int> Task(std::istream &&is)
{
    std::vector<std::string> map;
    std::string line;
    while (getline(is, line)) {
        map.push_back(std::move(line));
    }
    int height = map.size();
    int width = map[0].size();

    std::unordered_set<Pos, Pos::Hash> visited;

    auto getColor = [&](int x, int y) -> char {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return -1;
        return map[y][x];
    };

    int task1{}, task2{};
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            char c = map[row][col];

            int area{}, perimeter{}, edges{};
            auto paint = [&](int x, int y, auto &&paint) {
                if (visited.contains({x, y}) || getColor(x, y) != c)
                    return false;
                visited.insert({x, y});
                ++area;
                bool up = getColor(x, y - 1) == c;
                bool right = getColor(x + 1, y) == c;
                bool down = getColor(x, y + 1) == c;
                bool left = getColor(x - 1, y) == c;
                perimeter += 4 - (up + right + down + left);

                auto upRight = !(up || right) || (up && right && getColor(x + 1, y - 1) != c);
                auto downRight = !(down || right) || (down && right && getColor(x + 1, y + 1) != c);
                auto downLeft = !(down || left) || (down && left && getColor(x - 1, y + 1) != c);
                auto upLeft = !(up || left) || (up && left && getColor(x - 1, y - 1) != c);
                edges += upRight + downRight + downLeft + upLeft;

                paint(x - 1, y, paint);
                paint(x + 1, y, paint);
                paint(x, y - 1, paint);
                paint(x, y + 1, paint);
                return true;
            };

            if (paint(col, row, paint)) {
                task1 += area * perimeter;
                task2 += area * edges;
            }
        }
    }

    return {task1, task2};
}

suite s = [] {
    "12"_test = [] {
        const char *const TEST1 = R"(RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE)";
        auto test = Task(std::istringstream{TEST1});
        expect(1930_i == test.first);
        expect(1206_i == test.second);

        const char *const TEST2 = R"(AAAA
BBCD
BBCC
EEEC)";
        expect(80_i == Task(std::istringstream{TEST2}).second);

        const char *const TEST3 = R"(EEEEE
EXXXX
EEEEE
EXXXX
EEEEE)";
        expect(236_i == Task(std::istringstream{TEST3}).second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
