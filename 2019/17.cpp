#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>
#include <sstream>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCode prog{ifs};

    std::ostringstream oss;
    while (prog.GetState() != prog.S_HALT)
    {
        auto r = prog.Advance(0);
        switch (prog.GetState())
        {
        case prog.S_RUN:
        case prog.S_HALT:
            break;
        case prog.S_INPUT:
            REQUIRE(!"Not supported");
            break;
        case prog.S_OUTPUT:
            oss << char(r);
            break;
        }
    }

    std::string map = oss.str();
    MESSAGE("\n" << map);

    auto width = map.find("\n");
    auto height = map.size() / width;

    auto getTile = [&](int row, int col) {
        return map[row * (width + 1) + col];
    };

    auto isIntersection = [&](int row, int col) {
        return getTile(row, col) == '#'
            && getTile(row - 1, col) == '#'
            && getTile(row + 1, col) == '#'
            && getTile(row, col - 1) == '#'
            && getTile(row, col + 1) == '#';
    };

    int cs{};
    for (auto row = 1; row < height - 1; ++row)
    {
        for (auto col = 1; col < width - 1; ++col)
        {
            if (isIntersection(row, col))
                cs += row * col;
        }
    }
    MESSAGE(cs);
}
