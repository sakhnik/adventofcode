#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

using NumT = int64_t;
using VecT = std::vector<NumT>;

struct Expansion
{
    VecT coords, mapping;

    Expansion(NumT mult, const VecT &c)
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
                mapping.push_back(mapping.back() + (d - 1) * mult + 1);
        }
    }

    NumT Get(NumT coord) const
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

    NumT Task1() const
    {
        return Impl(2);
    }

    NumT Impl(NumT mult) const
    {
        Expansion er{mult, rows}, ec{mult, cols};
        VecT expanded_rows(rows.size()), expanded_cols(cols.size());
        for (int i = 0; i < rows.size(); ++i)
        {
            expanded_rows[i] = er.Get(rows[i]);
            expanded_cols[i] = ec.Get(cols[i]);
        }

        NumT sum{};
        for (int i = 1; i < rows.size(); ++i)
            for (int j = 0; j < i; ++j)
                sum += std::abs(expanded_rows[i] - expanded_rows[j]) + std::abs(expanded_cols[i] - expanded_cols[j]);
        return sum;
    }

private:
    VecT rows, cols;
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
        expect(1030_i == test1.Impl(10));
        expect(8410_i == test1.Impl(100));

        Cosmos cosmos{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", cosmos.Task1());
        Printer::Print(__FILE__, "2", cosmos.Impl(1000000));
    };
};

} //namespace;
