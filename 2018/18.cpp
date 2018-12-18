#include <doctest/doctest.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>

namespace {

class Area
{
public:
    Area(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            if (_area.empty())
            {
                _area.emplace_back(line.size() + 2 , ' ');
            }
            _area.push_back(' ' + line + ' ');
        }
        _area.emplace_back(_area.back().size() + 2, ' ');
        _prev_area = _area;
    }

    std::string Dump(size_t margin = 1) const
    {
        std::ostringstream oss;
        for (size_t row{margin}, nRow{_area.size() - margin}; row < nRow; ++row)
        {
            for (size_t col{margin}, nCol{_area[row].size() - margin}; col < nCol; ++col)
            {
                oss << _area[row][col];
            }
            oss << '\n';
        }
        return oss.str();
    }

    void Evolve(int count = 1)
    {
        for (; count > 0; --count)
        {
            _Evolve();
        }
    }

    size_t GetValue() const
    {
        return _Count('|') * _Count('#');
    }

private:
    std::vector<std::string> _area;
    std::vector<std::string> _prev_area;

    int _Count(size_t row, size_t col, char kind) const
    {
        int ret{};

        for (size_t r{row - 1}; r <= row + 1; ++r)
        {
            for (size_t c{col - 1}; c <= col + 1; ++c)
            {
                if (r == row && c == col)
                    continue;
                if (_area[r][c] == kind)
                    ++ret;
            }
        }

        return ret;
    }

    void _Evolve()
    {
        for (size_t row = 1; row < _area.size() - 1; ++row)
        {
            for (size_t col = 1; col < _area[row].size() - 1; ++col)
            {
                char cur = _area[row][col];
                switch (cur)
                {
                case '.':
                    _prev_area[row][col] = _Count(row, col, '|') >= 3 ? '|' : cur;
                    break;
                case '|':
                    _prev_area[row][col] = _Count(row, col, '#') >= 3 ? '#' : cur;
                    break;
                case '#':
                    {
                        bool has_lumber = _Count(row, col, '#') >= 1;
                        bool has_trees = _Count(row, col, '|') >= 1;
                        _prev_area[row][col] = (has_lumber && has_trees) ? '#' : '.';
                    }
                }
            }
        }

        _area.swap(_prev_area);
    }

    size_t _Count(char kind) const
    {
        size_t ret{};
        for (const auto &line : _area)
        {
            ret += count(begin(line), end(line), kind);
        }
        return ret;
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        Area a(std::istringstream{
            ".#.#...|#.\n"
            ".....#|##|\n"
            ".|..|...#.\n"
            "..|#.....#\n"
            "#.#|||#|#|\n"
            "...#.||...\n"
            ".|....|...\n"
            "||...#|.#|\n"
            "|.||||..|.\n"
            "...#.|..|.\n"
            });

        a.Evolve();
        std::string exp =
            ".......##.\n"
            "......|###\n"
            ".|..|...#.\n"
            "..|#||...#\n"
            "..##||.|#|\n"
            "...#||||..\n"
            "||...|||..\n"
            "|||||.||.|\n"
            "||||||||||\n"
            "....||..|.\n";
        REQUIRE(exp == a.Dump());

        a.Evolve();
        exp =
            ".......#..\n"
            "......|#..\n"
            ".|.|||....\n"
            "..##|||..#\n"
            "..###|||#|\n"
            "...#|||||.\n"
            "|||||||||.\n"
            "||||||||||\n"
            "||||||||||\n"
            ".|||||||||\n";
        REQUIRE(exp == a.Dump());

        a.Evolve();
        exp =
            ".......#..\n"
            "....|||#..\n"
            ".|.||||...\n"
            "..###|||.#\n"
            "...##|||#|\n"
            ".||##|||||\n"
            "||||||||||\n"
            "||||||||||\n"
            "||||||||||\n"
            "||||||||||\n";
        REQUIRE(exp == a.Dump());

        a.Evolve(7);
        exp =
            ".||##.....\n"
            "||###.....\n"
            "||##......\n"
            "|##.....##\n"
            "|##.....##\n"
            "|##....##|\n"
            "||##.####|\n"
            "||#####|||\n"
            "||||#|||||\n"
            "||||||||||\n";
        REQUIRE(exp == a.Dump());

        REQUIRE(1147 == a.GetValue());
    }

    SUBCASE("task") {
        Area a(std::ifstream{INPUT});
        a.Evolve(10);
        MESSAGE(a.GetValue());
    }
}
