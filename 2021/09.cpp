#include "../test.hpp"
#include <iterator>

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        std::string line;
        while (is >> line)
            _heights.push_back(line);
    }

    int CalcRiskLevel() const
    {
        int risk{};

        for (int row = 0; row < _heights.size(); ++row)
        {
            for (int col = 0; col < _heights[row].size(); ++col)
            {
                auto height = _heights[row][col];
                auto l = col > 0 ? _heights[row][col - 1] : std::numeric_limits<char>::max();
                auto u = row > 0 ? _heights[row - 1][col] : std::numeric_limits<char>::max();
                auto r = col < _heights[row].size() - 1 ? _heights[row][col + 1] : std::numeric_limits<char>::max();
                auto d = row < _heights.size() - 1 ? _heights[row + 1][col] : std::numeric_limits<char>::max();
                if (height < l && height < u && height < r && height < d)
                    risk += 1 + height - '0';
            }
        }

        return risk;
    }

    size_t CalcBasins() const
    {
        int rows = _heights.size();
        int cols = _heights[0].size();

        auto idx = [cols](int row, int col) {
            return row * cols + col;
        };

        std::vector<size_t> basins = {0};
        std::vector<size_t> visited(idx(rows, 0), 0);

        std::function<void(int, int, size_t)> visit;
        visit = [&, this](int row, int col, size_t basin_idx) {
            if (visited[idx(row, col)] || _heights[row][col] == '9')
                return;
            if (!basin_idx)
            {
                basins.push_back(0);
                basin_idx = basins.size() - 1;
            }
            visited[idx(row, col)] = basin_idx;
            ++basins[basin_idx];
            if (row > 0)
                visit(row - 1, col, basin_idx);
            if (row < rows - 1)
                visit(row + 1, col, basin_idx);
            if (col > 0)
                visit(row, col - 1, basin_idx);
            if (col < cols - 1)
                visit(row, col + 1, basin_idx);
        };

        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                visit(row, col, 0);
            }
        }

        std::sort(basins.begin(), basins.end(), [](size_t a, size_t b) { return a > b; });
        return basins[0] * basins[1] * basins[2];
    }

private:
    std::vector<std::string> _heights;
};

const char *TEST_INPUT = R"(
2199943210
3987894921
9856789892
8767896789
9899965678
)";

using namespace boost::ut;

suite s = [] {
    "2021-09"_test = [] {
        Map test_map{std::istringstream{TEST_INPUT}};
        expect(15_i == test_map.CalcRiskLevel());
        expect(1134_u == test_map.CalcBasins());

        Map m{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", m.CalcRiskLevel());
        Printer::Print(__FILE__, "2", m.CalcBasins());
    };
};

} //namespace;
