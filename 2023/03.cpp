#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

class Scheme
{
    std::vector<std::string> lines;

    struct Pos
    {
        int row = 0, col = 0;
        operator bool() const { return row || col; }
    };
    static constexpr auto pos_hash = [](const Pos &p) {
        return std::hash<int>()((p.row << 16) | p.col);
    };

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
        ForEachNumber([&sum, this](int row, int start, int end, const std::string &num) {
            if (HasSymbolAround(row, start, end, 0))
                sum += std::stoi(num);
        });
        return sum;
    }

    uint64_t Task2()
    {
        std::unordered_map<Pos, std::vector<int>, decltype(pos_hash)> gears{0, pos_hash};

        ForEachNumber([&](int row, int start, int end, const std::string &num) {
            auto pos = GetGearsAround(row, start, end);
            for (auto p : pos)
                gears[p].push_back(std::stoi(num));
        });
        uint64_t sum{};
        for (const auto &[_, nums] : gears)
        {
            if (nums.size() == 2)
                sum += nums[0] * nums[1];
        }
        return sum;
    }

private:
    template <typename ActionT>
    void ForEachNumber(ActionT action) const
    {
        static const std::regex r(R"((\d+))");
        using IterT = std::regex_iterator<std::string::const_iterator>;

        for (int row = 1, rowN = lines.size() - 1; row < rowN; ++row)
        {
            const auto &line = lines[row];
            for (IterT it(line.begin(), line.end(), r), itend; it != itend; ++it)
            {
                const auto &m = *it;
                action(row, m.position(0), m.position() + m.length(), m.str());
            }
        }
    }

    bool HasSymbolAround(int row, int start, int end, char sym)
    {
        return HasSymbolInSpan(row - 1, start - 1, end + 1, sym)
            || HasSymbolInSpan(row + 1, start - 1, end + 1, sym)
            || HasSymbol(row, start - 1, sym)
            || HasSymbol(row, end, sym);
    }

    bool HasSymbolInSpan(int row, int start, int end, char sym)
    {
        for (int col = start; col < end; ++col)
            if (HasSymbol(row, col, sym))
                return true;
        return false;
    }

    bool HasSymbol(int row, int col, char sym) const
    {
        char ch = lines[row][col];
        if (sym)
            return ch == sym;
        if (ch == '.')
            return false;
        if (ch >= 0 && ch <= 9)
            return false;
        return true;
    }

    std::vector<Pos> GetGearsAround(int row, int start, int end)
    {
        std::vector<Pos> gears = GetGearsInSpan(row - 1, start - 1, end + 1);
        auto gears_below = GetGearsInSpan(row + 1, start - 1, end + 1);
        gears.insert(gears.end(), gears_below.begin(), gears_below.end());
        if (lines[row][start - 1] == '*')
            gears.push_back({row, start - 1});
        if (lines[row][end] == '*')
            gears.push_back({row, end});
        return gears;
    }

    std::vector<Pos> GetGearsInSpan(int row, int start, int end)
    {
        std::vector<Pos> gears;
        for (int col = start; col < end; ++col)
            if (lines[row][col] == '*')
                gears.push_back({row, col});
        return gears;
    }
};

suite s = [] {
    "03"_test = [] {
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
        expect(467835_u == test_scheme.Task2());

        Scheme scheme{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", scheme.Task1());
        Printer::Print(__FILE__, "2", scheme.Task2());
    };
};

} //namespace;
