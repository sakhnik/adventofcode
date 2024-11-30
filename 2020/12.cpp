#include "../test.hpp"
#include <fstream>
#include <iostream>

namespace {

int Track(std::istream &&is)
{
    int x{}, y{};
    const char directions[] = {'E', 'S', 'W', 'N'};
    int dir = 0;

    auto move = [&](char d, int v) {
        switch (d)
        {
        case 'E': x += v; break;
        case 'S': y -= v; break;
        case 'W': x -= v; break;
        case 'N': y += v; break;
        }
    };

    char cmd{};
    int value{};
    while (is && (is >> cmd >> value))
    {
        switch (cmd)
        {
        case 'E':
        case 'S':
        case 'W':
        case 'N':
            move(cmd, value);
            break;
        case 'F':
            move(directions[dir], value);
            break;
        case 'R':
            dir = (dir + value / 90) % std::size(directions);
            break;
        case 'L':
            dir = (dir + 8 * std::size(directions) - value / 90) % std::size(directions);
            break;
        }
    }

    return std::abs(x) + std::abs(y);
}

int Track2(std::istream &&is)
{
    int x{}, y{};
    int wx{10}, wy{1};

    char cmd{};
    int value{};
    while (is && (is >> cmd >> value))
    {
        switch (cmd)
        {
        case 'E': wx += value; break;
        case 'S': wy -= value; break;
        case 'W': wx -= value; break;
        case 'N': wy += value; break;
        case 'L':
            for (; value > 0; value -= 90)
            {
                std::swap(wx, wy);
                wx = -wx;
            }
            break;
        case 'R':
            for (; value > 0; value -= 90)
            {
                std::swap(wx, wy);
                wy = -wy;
            }
            break;
        case 'F':
            x += wx * value;
            y += wy * value;
            break;
        }
    }

    return std::abs(x) + std::abs(y);
}

using namespace boost::ut;

suite s = [] {
    "12"_test = [] {
        const char *const TEST = "F10\nN3\nF7\nR90\nF11";
        expect(25_i == Track(std::istringstream{TEST}));
        expect(286_i == Track2(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", Track(std::ifstream{INPUT}));
        Printer::Print(__FILE__, "2", Track2(std::ifstream{INPUT}));
    };
};

} //namespace;
