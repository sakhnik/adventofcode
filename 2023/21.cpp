#include "../test.hpp"
#include <fstream>
#include <boost/functional/hash.hpp>

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
        size_t seed{0};
        boost::hash_combine(seed, p.row);
        boost::hash_combine(seed, p.col);
        return seed;
    };
    Pos start;

    std::vector<size_t> counts;

    static constexpr size_t TASK2_STEPS = 26501365;
    std::vector<int64_t> task2_counts;
    std::vector<int64_t> task2_speed;
    std::vector<int64_t> task2_accel;

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

        counts.push_back(1);
        Spread(1000);
    }

    void Spread(int steps)
    {
        const int height = map.size();
        const int width = map[0].size();

        using FrontierT = std::unordered_set<Pos, decltype(posHash)>;
        FrontierT a(0, posHash), b(0, posHash);
        FrontierT *q = &a, *qnext = &b;

        q->insert(start);

        for (int step = 1; step <= steps; ++step)
        {
            //std::cerr << " " << step << std::flush;
            for (auto pos : *q)
            {
                auto go = [&](int dr, int dc) {
                    Pos new_pos = {pos.row + dr, pos.col + dc};
                    int map_row = new_pos.row % height;
                    if (map_row < 0)
                        map_row += height;
                    int map_col = new_pos.col % width;
                    if (map_col < 0)
                        map_col += width;
                    if (map[map_row][map_col] != '.' && map[map_row][map_col] != 'S') return;
                    qnext->insert(new_pos);
                };
                go(1, 0);
                go(0, 1);
                go(-1, 0);
                go(0, -1);
            }
            std::swap(q, qnext);

            assert(width == height);
            counts.push_back(q->size());
            if (step % width == TASK2_STEPS % width)
            {
                //std::cerr << step << " " << q->size() << std::endl;
                task2_counts.push_back(q->size());
                if (task2_counts.size() > 1)
                    task2_speed.push_back(task2_counts.back() - task2_counts[task2_counts.size() - 2]);
                if (task2_speed.size() > 1)
                    task2_accel.push_back(task2_speed.back() - task2_speed[task2_speed.size() - 2]);
                if (task2_accel.size() >= 1)
                    return;
            }
        }
    }

    int64_t Task1(int steps)
    {
        return counts[steps];
    }

    int64_t Task2()
    {
        size_t periods = TASK2_STEPS / map.size();
        return task2_counts[0] + task2_speed[0] * periods + (periods * (periods - 1) / 2) * task2_accel[0];
    }
};

suite s = [] {
    "21"_test = [] {
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

        if (Printer::EnableAll())
        {
            Garden task{std::ifstream{INPUT}};
            Printer::Print(__FILE__, "1", task.Task1(64));
            Printer::Print(__FILE__, "2", task.Task2());
        }
    };
};

} //namespace;
