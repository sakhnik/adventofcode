#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

class Scheme
{
    std::vector<std::string> lines;

public:
    Scheme(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            if (lines.empty())
                lines.push_back(std::string(line.size() + 2, '.'));
            lines.push_back('.' + line + '.');
        }
        lines.push_back(std::string(lines.back().size() + 2, '.'));
    }

    int Task1()
    {
        int sum{};

        static const std::regex r(R"((\d+))");
        using IterT = std::regex_iterator<std::string::const_iterator>;

        for (int row = 1, rowN = lines.size() - 1; row < rowN; ++row)
        {
            const auto &line = lines[row];
            for (IterT it(line.begin(), line.end(), r), itend; it != itend; ++it)
            {
                const auto &m = *it;
                if (HasSymbolAround(row, m.position(), m.position() + m.length()))
                    sum += std::stoi(m.str());
            }
        }

        return sum;
    }

private:
    bool HasSymbolInSpan(int row, int start, int end)
    {
        for (int col = start; col < end; ++col)
            if (HasSymbol(row, col))
                return true;
        return false;
    }

    bool HasSymbolAround(int row, int start, int end)
    {
        return HasSymbolInSpan(row - 1, start - 1, end + 1)
            || HasSymbolInSpan(row + 1, start - 1, end + 1)
            || HasSymbol(row, start - 1)
            || HasSymbol(row, end);
    }

    bool HasSymbol(int row, int col) const
    {
        char ch = lines[row][col];
        if (ch == '.')
            return false;
        if (ch >= 0 && ch <= 9)
            return false;
        return true;
    }
};

using SchemeT = std::vector<std::string>;
suite s = [] {
    "2023-03"_test = [] {
        const char *const TEST1 = R"(467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
)";
        Scheme test_scheme{std::istringstream{TEST1}};
        expect(4361_i == test_scheme.Task1());
        //expect(2286_u == Task2(test_games));

        Scheme scheme{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", scheme.Task1());
        //Printer::Print(__FILE__, "2", Task2(games));
    };
};

} //namespace;
