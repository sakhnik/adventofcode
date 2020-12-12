#include "../test.hpp"

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

using namespace boost::ut;

suite s = [] {
    "2020-12"_test = [] {
        const char *const TEST = "F10\nN3\nF7\nR90\nF11";
        expect(25_i == Track(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", Track(std::ifstream{INPUT}));
    };
};

} //namespace;
