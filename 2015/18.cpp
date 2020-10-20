#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/ut.hpp>

namespace {

using namespace boost::ut;

class Life
{
public:
    Life(std::istream &&is, bool bug = false)
        : _bug(bug)
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
            expect(line.size() + 2 == _width);
            std::copy(line.begin(), line.end(),
                      _lights.begin() + _width * row + 1);
            ++row;
        }

        if (bug)
            _ApplyBug(_lights);
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
                    auto idx = _Idx(row, col);
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

            if (_bug)
            {
                _ApplyBug(next);
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
                std::cout << _lights[_Idx(row, col)];
            }
            std::cout << "\n";
        }
        std::cout << std::flush;
    }

private:
    bool _bug;
    size_t _width{};
    std::string _lights;

    void _ApplyBug(std::string &t) const
    {
        t[_Idx(1, 1)] = '#';
        t[_Idx(1, _width - 2)] = '#';
        t[_Idx(_width - 2, 1)] = '#';
        t[_Idx(_width - 2, _width - 2)] = '#';
    }

    size_t _Idx(size_t row, size_t col) const
    {
        return row * _width + col;
    }

    int _CountNeighbours(size_t row, size_t col) const
    {
        int res{};
        auto iup = _Idx(row - 1, col - 1);
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

const char *const TEST =
    ".#.#.#\n"
    "...##.\n"
    "#....#\n"
    "..#...\n"
    "#.#..#\n"
    "####..\n";
suite s = [] {
    "2015-18.test1"_test = [] {
        Life l(std::istringstream{TEST});
        expect(15_u == l.CountOn());
        //l.Animate(1);
        //REQUIRE(11 == l.CountOn());
        //l.Animate(1);
        //REQUIRE(8 == l.CountOn());
        //l.Animate(1);
        //REQUIRE(4 == l.CountOn());
        l.Animate(4);
        expect(4_u == l.CountOn());
    };

    "2015-18.task1"_test = [] {
        Life l(std::ifstream{INPUT});
        l.Animate(100);
        std::cout << "2015-18.1: " << l.CountOn() << std::endl;
    };

    "2015-18.test2"_test = [] {
        Life l(std::istringstream{TEST}, true);
        expect(17_u == l.CountOn());
        l.Animate(1);
        expect(18_u == l.CountOn());
        l.Animate(1);
        expect(18_u == l.CountOn());
        l.Animate(1);
        expect(18_u == l.CountOn());
        l.Animate(1);
        expect(14_u == l.CountOn());
        l.Animate(1);
        expect(17_u == l.CountOn());
    };

    "2015-18.task2"_test = [] {
        Life l(std::ifstream{INPUT}, true);
        l.Animate(100);
        std::cout << "2015-18.2: " << l.CountOn() << std::endl;
    };
};

} //namespace;