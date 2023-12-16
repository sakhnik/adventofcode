#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Mirrors
{
    std::vector<std::string> map;

    Mirrors(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
            map.push_back(line);
    }

    int Energize(int start_row, int start_col, int start_dir) const
    {
        std::vector<std::vector<int>> energized(map.size(), std::vector<int>(map[0].size(), 0));

        const int dx[] = {1, 0, -1, 0};
        const int dy[] = {0, 1, 0, -1};

        auto track = [&](int row, int col, int dir, auto &&track) -> void {
            auto step = [&](int d) {
                track(row + dy[d], col + dx[d], d, track);
            };

            if (row < 0 || row >= map.size()) return;
            if (col < 0 || col >= map.size()) return;
            auto dir_mask = 1 << dir;
            if (energized[row][col] & dir_mask) return;
            energized[row][col] |= dir_mask;
            switch (map[row][col])
            {
            case '.': step(dir); break;
            case '|':
                if (dir == 1 || dir == 3)
                    step(dir);
                else
                {
                    step((dir + 1) % 4);
                    step((dir + 3) % 4);
                }
                break;
            case '-':
                if (dir == 0 || dir == 2)
                    step(dir);
                else
                {
                    step((dir + 1) % 4);
                    step((dir + 3) % 4);
                }
                break;
            case '/': step(3 - dir); break;
            case '\\':
                {
                    constexpr const int turn[] = {1, 0, 3, 2};
                    step(turn[dir]);
                }
                break;
            }
        };
        track(start_row, start_col, start_dir, track);

        int sum{};
        for (int row = 0; row < energized.size(); ++row)
        {
            for (int col = 0, colN = energized[0].size(); col < colN; ++col)
            {
                if (energized[row][col])
                    ++sum;
            }
        }
        return sum;
    }

    int Task1() const
    {
        return Energize(0, 0, 0);
    }

    int Task2() const
    {
        auto map_width = map[0].size();

        int max{};
        for (int row = 0; row < map.size(); ++row)
        {
            max = std::max(max, Energize(row, 0, 0));
            max = std::max(max, Energize(row, map_width - 1, 2));
        }
        for (int col = 0; col < map[0].size(); ++col)
        {
            max = std::max(max, Energize(0, col, 1));
            max = std::max(max, Energize(map.size() - 1, col, 3));
        }
        return max;
    }
};

suite s = [] {
    "2023-16"_test = [] {
        const char *const TEST1 = R"(.|...\....
|.-.\.....
.....|-...
........|.
..........
.........\
..../.\\..
.-.-/..|..
.|....-|.\
..//.|....
)";
        Mirrors test1{std::istringstream{TEST1}};
        expect(46_i == test1.Task1());
        expect(51_i == test1.Task2());

        Mirrors mirrors{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", mirrors.Task1());
        Printer::Print(__FILE__, "2", mirrors.Task2());
    };
};

} //namespace;
