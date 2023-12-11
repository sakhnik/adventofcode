#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Expansion
{
    std::vector<int> coords, mapping;

    Expansion(const std::vector<int> &c)
        : coords{std::move(c)}
    {
        std::sort(coords.begin(), coords.end());
        mapping.push_back(0);
        for (int i = 1; i < coords.size(); ++i)
        {
            auto d = coords[i] - coords[i - 1];
            if (d <= 1)
                mapping.push_back(mapping.back() + d);
            else
                mapping.push_back(mapping.back() + d + d - 1);
        }
    }

    int Get(int coord) const
    {
        auto idx = std::lower_bound(coords.begin(), coords.end(), coord) - coords.begin();
        return mapping[idx];
    }
};

class Cosmos
{
public:
    Cosmos(std::istream &&is)
    {
        std::string line;
        int row{};
        while (getline(is, line))
        {
            for (int col = 0; col < line.size(); ++col)
                if (line[col] == '#')
                {
                    rows.push_back(row);
                    cols.push_back(col);
                }
            ++row;
        }
    }

    int Task1() const
    {
        Expansion er{rows}, ec{cols};
        std::vector<int> expanded_rows(rows.size()), expanded_cols(cols.size());
        for (int i = 0; i < rows.size(); ++i)
        {
            expanded_rows[i] = er.Get(rows[i]);
            expanded_cols[i] = ec.Get(cols[i]);
        }

        int sum{};
        for (int i = 1; i < rows.size(); ++i)
            for (int j = 0; j < i; ++j)
                sum += std::abs(expanded_rows[i] - expanded_rows[j]) + std::abs(expanded_cols[i] - expanded_cols[j]);
        return sum;
    }

private:
    std::vector<int> rows, cols;
};

suite s = [] {
    "2023-11"_test = [] {
        const char *const TEST1 = R"(...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....
)";
        Cosmos test1{std::istringstream{TEST1}};
        expect(374_i == test1.Task1());
        //expect(1_i == test1.Task2());

        Cosmos cosmos{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", cosmos.Task1());
        //Printer::Print(__FILE__, "2", pipe.Task2());
    };
};

} //namespace;
