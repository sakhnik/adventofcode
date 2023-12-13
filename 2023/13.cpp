#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Mirrors
{
    using PatternT = std::vector<std::string>;

    int task1{};
    int task2{};

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
        int ignore_horiz{-1}, ignore_vert{-1};
        task1 += FindMirror(pattern, ignore_horiz, ignore_vert);
        task2 += FindSmudge(pattern, ignore_horiz, ignore_vert);
    }

    int FindMirror(const PatternT &pattern, int &ignore_horiz, int &ignore_vert)
    {
        auto horz = FindHorizMirror(pattern, ignore_horiz);
        if (horz != -1)
        {
            ignore_horiz = horz;
            return horz * 100;
        }
        else
        {
            auto rotated = Rotate(pattern);
            auto vert = FindHorizMirror(rotated, ignore_vert);
            if (vert != -1)
            {
                ignore_vert = vert;
                return vert;
            }
        }
        assert(false);
        return -1;
    }

    int FindSmudge(PatternT pattern, const int &ignore_horiz, const int &ignore_vert)
    {
        auto mirror = FindSmudgeImpl(pattern, ignore_horiz);
        if (mirror != -1)
            return mirror * 100;
        auto rotated = Rotate(pattern);
        mirror = FindSmudgeImpl(rotated, ignore_vert);
        return mirror;
    }

    int FindSmudgeImpl(PatternT pattern, const int &ignore)
    {
        int colN = pattern[0].size();
        for (int row = 0; row < pattern.size(); ++row)
        {
            for (int col = 0; col < colN; ++col)
            {
                auto &cell = pattern[row][col];
                auto prev = cell;
                cell = cell == '#' ? '.' : '#';
                auto mirror = FindHorizMirror(pattern, ignore);
                if (mirror != -1 && std::abs(mirror - row) * 2 < pattern.size())
                    return mirror;
                cell = prev;
            }
        }
        return -1;
    }

    int Task1() const
    {
        return task1;
    }

    int Task2() const
    {
        return task2;
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

    int FindHorizMirror(const PatternT &pattern, const int &ignore)
    {
        auto mid = pattern.size() / 2;
        for (auto i = 0; i < pattern.size(); ++i)
        {
            auto top = mid + i / 2 * ((i & 1) * 2 - 1);
            if (top + 1 != ignore && IsMirror(pattern, top))
                return top + 1;
        }
        return -1;
    }

    PatternT Rotate(const PatternT &pattern)
    {
        int colN = pattern[0].size();
        PatternT rotated(colN);
        for (int row = 0; row < pattern.size(); ++row)
            for (int col = 0; col < colN; ++col)
                rotated[col].push_back(pattern[row][col]);
        return rotated;
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
        expect(400_i == test1.Task2());

        Mirrors mirrors{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", mirrors.Task1());
        Printer::Print(__FILE__, "2", mirrors.Task2());
    };
};

} //namespace;
