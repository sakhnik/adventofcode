#include <doctest/doctest.h>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <regex>
#include <iostream>

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            _Pos pos{};

            static const std::regex r{R"(([xy])=(\d+), ([xy])=(\d+)\.\.(\d+))"};
            std::smatch m;
            REQUIRE(std::regex_search(line, m, r));
            REQUIRE(m.size() == 6);

            auto &x = m[1] == 'x' ? pos.col : pos.row;
            auto &y = m[3] == 'y' ? pos.row : pos.col;
            x = std::stoi(m[2]);
            for (int i = stoi(m[4]), n = stoi(m[5]); i <= n; ++i)
            {
                y = i;
                _map.emplace(pos, '#');
            }
        }

        for (const auto &m : _map)
        {
            const auto &p = m.first;
            if (p.col - 1 < _topleft.col)
                _topleft.col = p.col - 1;
            if (p.col + 1 > _bottomright.col)
                _bottomright.col = p.col + 1;
            if (p.row > _bottomright.row)
                _bottomright.row = p.row;
            if (p.row < _ymin)
                _ymin = p.row;
        }
    }

    void Print() const
    {
        for (_IntT row = 0; row <= _bottomright.row; ++row)
        {
            for (_IntT col = _topleft.col; col <= _bottomright.col; ++col)
            {
                std::cout << _Get({row, col});
            }
            std::cout << "\n";
        }
        std::cout << std::flush;
    }

    void Pour()
    {
        _Pour({0, 500});
        _map[{0, 500}] = '+';
    }

    int CountWater() const
    {
        return std::count_if(begin(_map), end(_map),
                             [this](const auto &t) {
                                return t.first.row >= _ymin && (t.second == '|' || t.second == '~');
                             });
    }

private:
    using _IntT = int;
    using _LimitsT = std::numeric_limits<_IntT>;
    struct _Pos
    {
        _IntT row;
        _IntT col;

        bool operator==(const _Pos &o) const
        {
            return row == o.row && col == o.col;
        }

        bool operator<(const _Pos &o) const
        {
            return row < o.row || (row == o.row && col < o.col);
        }

        struct Hasher
        {
            std::size_t operator()(const _Pos &p) const
            {
                return std::hash<_IntT>()(p.row << 16 | p.col);
            }
        };
    };
    using _MapT = std::unordered_map<_Pos, char, _Pos::Hasher>;
    _MapT _map;

    _IntT _ymin = _LimitsT::max();
    _Pos _topleft = {0, _LimitsT::max()};
    _Pos _bottomright = {_LimitsT::min(), _LimitsT::min()};

    char _Get(_Pos pos) const
    {
        auto it = _map.find(pos);
        return it != end(_map) ? it->second : '.';
    }

    bool _Pour(_Pos pos)
    {
        char cur = _Get(pos);
        if (cur == '|')
            return true;
        if (cur == '#')
            return false;
        if (cur == '~')
            return false;

        _map[pos] = '|';
        if (pos.row >= _bottomright.row)
            return true;

        if (_Pour({pos.row + 1, pos.col}))
            return true;

        bool left = _Spill({pos.row, pos.col - 1}, -1);
        bool right = _Spill({pos.row, pos.col + 1}, 1);
        if (!left && !right)
        {
            _map[pos] = '~';
            for (_IntT col = pos.col - 1; _Get({pos.row, col}) != '#'; --col)
                _map[{pos.row, col}] = '~';
            for (_IntT col = pos.col + 1; _Get({pos.row, col}) != '#'; ++col)
                _map[{pos.row, col}] = '~';
            return false;
        }

        return true;
    }

    bool _Spill(_Pos pos, _IntT dir)
    {
        if (_Get(pos) == '#')
            return false;
        _map[pos] = '|';
        if (_Pour({pos.row + 1, pos.col}))
            return true;
        return _Spill({pos.row, pos.col + dir}, dir);
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        const char *const TEST =
R"(x=495, y=2..7
y=7, x=495..501
x=501, y=3..7
x=498, y=2..4
x=506, y=1..2
x=498, y=10..13
x=504, y=10..13
y=13, x=498..504
)";
        Map m(std::istringstream{TEST});
        m.Pour();
        m.Print();
        REQUIRE(57 == m.CountWater());
    }

    SUBCASE("task") {
        Map m(std::ifstream{INPUT});
        m.Pour();
        //m.Print();
        MESSAGE(m.CountWater());
    }
}
