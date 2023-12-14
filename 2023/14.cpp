#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Stones
{
    using MapT = std::vector<std::string>;
    MapT map;

    Stones(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
            map.push_back(line);
        Shake();
    }

    void North()
    {
        for (int col = 0, colN = map[0].size(); col < colN; ++col)
        {
            int bottom{0};
            for (int cur{bottom}; cur < map.size(); ++cur)
            {
                switch (map[cur][col])
                {
                case '.': break;
                case '#': bottom = cur + 1; break;
                case 'O': std::swap(map[bottom++][col], map[cur][col]); break;
                }
            }
        }
    }

    void West()
    {
        for (int row = 0; row < map.size(); ++row)
        {
            int bottom{0};
            for (int cur{bottom}, colN = map[0].size(); cur < colN; ++cur)
            {
                switch (map[row][cur])
                {
                case '.': break;
                case '#': bottom = cur + 1; break;
                case 'O': std::swap(map[row][bottom++], map[row][cur]); break;
                }
            }
        }
    }

    void South()
    {
        for (int col = 0, colN = map[0].size(); col < colN; ++col)
        {
            int bottom = map.size() - 1;
            for (int cur{bottom}; cur >= 0; --cur)
            {
                switch (map[cur][col])
                {
                case '.': break;
                case '#': bottom = cur - 1; break;
                case 'O': std::swap(map[bottom--][col], map[cur][col]); break;
                }
            }
        }
    }

    void East()
    {
        for (int row = 0; row < map.size(); ++row)
        {
            int bottom = map[0].size() - 1;
            for (int cur{bottom}; cur >= 0; --cur)
            {
                switch (map[row][cur])
                {
                case '.': break;
                case '#': bottom = cur - 1; break;
                case 'O': std::swap(map[row][bottom--], map[row][cur]); break;
                }
            }
        }
    }

    static int CalcLoad(const MapT &m)
    {
        int sum{};
        for (int row = 0; row < m.size(); ++row)
        {
            const auto &level = m[row];
            sum += (m.size() - row) * std::count(level.begin(), level.end(), 'O');
        }
        return sum;
    }

    int task1{};
    std::map<MapT, int> configs;
    std::map<int, MapT> lookup;
    int loop_start{};
    int loop_period{};

    void Shake()
    {
        for (int i = 0; ; ++i)
        {
            auto it = configs.find(map);
            if (it != configs.end())
            {
                loop_start = it->second;
                loop_period = i - loop_start;
                break;
            }
            configs[map] = i;
            lookup[i] = map;

            North();
            if (!i)
                task1 = CalcLoad(map);
            West();
            South();
            East();
        }
    }

    int Task1() const
    {
        return task1;
    }

    int Task2() const
    {
        int phase = (1000000000 - loop_start) % loop_period + loop_start;
        return CalcLoad(lookup.at(phase));
    }
};

suite s = [] {
    "2023-14"_test = [] {
        const char *const TEST1 = R"(O....#....
O.OO#....#
.....##...
OO.#O....O
.O.....O#.
O.#..O.#.#
..O..#O..O
.......O..
#....###..
#OO..#....
)";
        Stones test1{std::istringstream{TEST1}};
        expect(136_i == test1.Task1());
        expect(64_i == test1.Task2());

        Stones stones{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", stones.Task1());
        Printer::Print(__FILE__, "2", stones.Task2());
    };
};

} //namespace;
