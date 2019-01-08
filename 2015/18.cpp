#include <doctest/doctest.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

namespace {

class Life
{
public:
    Life(std::istream &&is)
    {
        size_t row{1};
        std::string line;
        while (is && getline(is, line))
        {
            if (!_width)
            {
                _width = line.size() + 2;
                _lights.resize(_width * _width, '.');
            }
            REQUIRE(line.size() + 2 == _width);
            std::copy(line.begin(), line.end(),
                      _lights.begin() + _width * row + 1);
            ++row;
        }
    }

    size_t CountOn() const
    {
        return std::count(_lights.begin(), _lights.end(), '#');
    }

    void Animate(int count)
    {
        std::string next{_lights};

        while (count--)
        {
            for (size_t row = 1; row + 1 < _width; ++row)
            {
                for (size_t col = 1; col + 1 < _width; ++col)
                {
                    auto idx = row * _width + col;
                    bool is_on = _lights[idx] == '#';
                    int neighb = _CountNeighbours(row, col);
                    if (is_on)
                    {
                        next[idx] = (neighb != 2 && neighb != 3) ? '.' : _lights[idx];
                    }
                    else
                    {
                        next[idx] = neighb == 3 ? '#' : _lights[idx];
                    }
                }
            }

            _lights.swap(next);

            if (_lights == next)
            {
                break;
            }
        }
    }

    void Print() const
    {
        for (size_t row = 1; row + 1 < _width; ++row)
        {
            for (size_t col = 1; col + 1 < _width; ++col)
            {
                std::cout << _lights[row * _width + col];
            }
            std::cout << "\n";
        }
        std::cout << std::flush;
    }

private:
    size_t _width{};
    std::string _lights;

    int _CountNeighbours(size_t row, size_t col) const
    {
        int res{};
        auto iup = _width * (row - 1) + col - 1;
        for (int i = 0; i < 3; ++i)
            res += _lights[iup + i] == '#';
        res += _lights[iup + _width] == '#';
        res += _lights[iup + _width + 2] == '#';
        auto idown = iup + _width + _width;
        for (int i = 0; i < 3; ++i)
            res += _lights[idown + i] == '#';
        return res;
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        const char *const TEST =
            ".#.#.#\n"
            "...##.\n"
            "#....#\n"
            "..#...\n"
            "#.#..#\n"
            "####..\n";
        Life l(std::istringstream{TEST});
        REQUIRE(15 == l.CountOn());
        //l.Animate(1);
        //REQUIRE(11 == l.CountOn());
        //l.Animate(1);
        //REQUIRE(8 == l.CountOn());
        //l.Animate(1);
        //REQUIRE(4 == l.CountOn());
        l.Animate(4);
        REQUIRE(4 == l.CountOn());
    }

    SUBCASE("task") {
        Life l(std::ifstream{INPUT});
        l.Animate(100);
        MESSAGE(l.CountOn());
    }
}
