#include "../test.hpp"

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

        Map m{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", m.CalcRiskLevel());
    };
};

} //namespace;
