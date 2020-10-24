#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "../test.hpp"

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
        _history.emplace(Dump(), 0);
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

    size_t Evolve(size_t count = 1)
    {
        for (; count > 0; --count)
        {
            if (_period)
            {
                auto target = _start + (_round + count - _start) % _period;
                auto it = std::find_if(begin(_history), end(_history),
                                       [target](const auto &h) { return h.second == target; });
                _round += count;
                return GetValue2(it->first);
            }

            _Evolve();
            ++_round;

            auto dump = Dump();
            auto it = _history.find(dump);
            if (it != end(_history) && !_period)
            {
                _start = it->second;
                _period = _round - it->second;
            }
            else
            {
                _history.emplace(std::move(dump), _round);
            }
        }
        return GetValue();
    }

    size_t GetValue() const
    {
        return _Count('|') * _Count('#');
    }

    static size_t GetValue2(const std::string &area)
    {
        return count(begin(area), end(area), '|') * count(begin(area), end(area), '#');
    }

private:
    using _AreaT = std::vector<std::string>;
    _AreaT _area;
    _AreaT _prev_area;
    std::unordered_map<std::string, size_t> _history;
    size_t _round = 0;
    size_t _period = 0;
    size_t _start = 0;

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

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2018-18"_test = [] {
        {
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
                "...#.|..|.\n"});

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
            expect(eq(exp, a.Dump()));

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
            expect(eq(exp, a.Dump()));

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
            expect(eq(exp, a.Dump()));

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
            expect(eq(exp, a.Dump()));

            expect(1147_u == a.GetValue());
        }

        Area a(std::ifstream{INPUT});
        a.Evolve(10);
        Printer::Print(__FILE__, "1", a.GetValue());
        Printer::Print(__FILE__, "2", a.Evolve(1000000000 - 10));
    };
};

} //namespace;