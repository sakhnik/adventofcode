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
    }

    int GetVal(int row) const
    {
        return map.size() - row;
    }

    int CalcColumn(int col) const
    {
        int sum{};

        int bottom{0};
        for (int cur{bottom}; cur < map.size(); ++cur)
        {
            switch (map[cur][col])
            {
            case '.': continue;
            case '#': bottom = cur + 1; continue;
            case 'O': sum += GetVal(bottom++); continue;
            }
        }

        return sum;
    }

    int Task1() const
    {
        int sum{};
        for (int col = 0, colN = map[0].size(); col < colN; ++col)
            sum += CalcColumn(col);
        return sum;
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
        //expect(400_i == test1.Task2());

        Stones stones{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", stones.Task1());
        //Printer::Print(__FILE__, "2", mirrors.Task2());
    };
};

} //namespace;
