#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Garden
{
    std::vector<std::string> map;
    struct Pos
    {
        int row{}, col{};
        bool operator==(const Pos &o) const {
            return row == o.row && col == o.col;
        }
    };
    static constexpr const auto posHash = [](const Pos &p) {
        return std::hash<size_t>()((p.row << 16) | p.col);
    };
    Pos start;

    Garden(std::istream &&is)
    {
        std::string line;
        int row{};
        while (getline(is, line))
        {
            auto idx = line.find('S');
            if (idx != line.npos)
            {
                start.row = row;
                start.col = idx;
            }
            map.push_back(line);
            ++row;
        }
    }

    int64_t Task1(int steps)
    {
        using FrontierT = std::unordered_set<Pos, decltype(posHash)>;
        FrontierT a(0, posHash), b(0, posHash);
        FrontierT *q = &a, *qnext = &b;

        q->insert(start);
        while (steps--)
        {
            while (!q->empty())
            {
                auto pos = *q->begin();
                q->erase(q->begin());

                auto go = [&](int dr, int dc) {
                    Pos new_pos = {pos.row + dr, pos.col + dc};
                    if (new_pos.row < 0 || new_pos.row >= map.size()) return;
                    if (new_pos.col < 0 || new_pos.col >= map[0].size()) return;
                    if (map[new_pos.row][new_pos.col] == '#') return;
                    qnext->insert(new_pos);
                };
                go(1, 0);
                go(0, 1);
                go(-1, 0);
                go(0, -1);
            }
            std::swap(q, qnext);
            qnext->clear();
        }
        return q->size();
    }

    int64_t Task2()
    {
        return 0;
    }
};

suite s = [] {
    "2023-21"_test = [] {
        const char *const TEST1 = R"(...........
.....###.#.
.###.##..#.
..#.#...#..
....#.#....
.##..S####.
.##..#...#.
.......##..
.##.#.####.
.##..##.##.
...........
)";
        Garden test1{std::istringstream{TEST1}};
        expect(16_i == test1.Task1(6));

        Garden task{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", task.Task1(64));
        //Printer::Print(__FILE__, "2", task.Task2());
    };
};

} //namespace;
