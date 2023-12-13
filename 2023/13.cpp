#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Mirrors
{
    using PatternT = std::vector<std::string>;

    int task1{};

    Mirrors(std::istream &&is)
    {
        PatternT pattern;
        std::string line;
        while (getline(is, line))
        {
            if (!line.empty())
            {
                pattern.push_back(line);
                continue;
            }
            ProcessPattern(pattern);
            pattern.clear();
        }
        ProcessPattern(pattern);
    }

    void ProcessPattern(const PatternT &pattern)
    {
        auto horz = FindHoriz(pattern);
        if (horz != -1)
        {
            task1 += horz * 100;
        }
        else
        {
            auto vert = FindVert(pattern);
            if (vert != -1)
            {
                task1 += vert;
            }
        }
    }

    int Task1() const
    {
        return task1;
    }

    bool IsMirror(const PatternT &pattern, size_t top)
    {
        for (int i = top, j = top + 1; i >= 0 && j < pattern.size(); --i, ++j)
        {
            if (pattern[i] != pattern[j])
                return false;
        }
        return true;
    }

    int FindHoriz(const PatternT &pattern)
    {
        auto mid = pattern.size() / 2;
        for (auto i = 0; i < pattern.size(); ++i)
        {
            auto top = mid + i / 2 * ((i & 1) * 2 - 1);
            if (IsMirror(pattern, top))
                return top + 1;
        }
        return -1;
    }

    int FindVert(const PatternT &pattern)
    {
        int colN = pattern[0].size();
        PatternT rotated(colN);
        for (int row = 0; row < pattern.size(); ++row)
            for (int col = 0; col < colN; ++col)
                rotated[col].push_back(pattern[row][col]);
        return FindHoriz(rotated);
    }
};

suite s = [] {
    "2023-13"_test = [] {
        const char *const TEST1 = R"(#.##..##.
..#.##.#.
##......#
##......#
..#.##.#.
..##..##.
#.#.##.#.

#...##..#
#....#..#
..##..###
#####.##.
#####.##.
..##..###
#....#..#
)";
        Mirrors test1{std::istringstream{TEST1}};
        expect(405_i == test1.Task1());
        //expect(525152_u == test1.Task2());

        Mirrors mirrors{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", mirrors.Task1());
        //Printer::Print(__FILE__, "2", springs.Task2());
    };
};

} //namespace;
