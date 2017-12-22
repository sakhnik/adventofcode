#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <set>
#include <fstream>

typedef std::pair<int, int> CoordT;
typedef std::set<CoordT> GridT;

GridT Parse(std::istream &is)
{
    GridT grid;

    int centre{0};
    int y = 0;
    std::string line;
    while (getline(is, line))
    {
        if (!centre)
            centre = line.size() / 2;
        for (int x = 0; x < static_cast<int>(line.size()); ++x)
        {
            if (line[x] == '#')
                grid.insert({x - centre, y - centre});
        }
        ++y;
    }

    return grid;
}

unsigned Count(unsigned iterations, GridT grid)
{
    unsigned count{0};
    CoordT pos{0, 0};
    enum Direction
    {
        UP = 0,
        RIGHT,
        DOWN,
        LEFT,

        DIRECTION_COUNT
    } direction{UP};

    while (iterations--)
    {
        auto it = grid.find(pos);
        bool is_infected = it != grid.end();
        direction = static_cast<Direction>((direction + DIRECTION_COUNT + (is_infected ? 1 : -1)) % DIRECTION_COUNT);
        if (is_infected)
            grid.erase(it);
        else
        {
            grid.insert(pos);
            ++count;
        }
        switch (direction)
        {
        case UP:
            --pos.second;
            break;
        case RIGHT:
            ++pos.first;
            break;
        case DOWN:
            ++pos.second;
            break;
        case LEFT:
            --pos.first;
            break;
        default:
            REQUIRE(false);
        }
    }

    return count;
}

TEST_CASE("1")
{
    std::istringstream iss("..#\n#..\n...");
    auto grid = Parse(iss);
    REQUIRE(grid.size() == 2);
    REQUIRE(grid.count({-1,0}) == 1);
    REQUIRE(grid.count({1,-1}) == 1);

    CHECK(Count(7, grid) == 5);
    CHECK(Count(70, grid) == 41);
    CHECK(Count(10000, grid) == 5587);
}

TEST_CASE("main")
{
    std::ifstream ifs(INPUT);
    auto grid = Parse(ifs);
    std::cout << Count(10000, grid) << std::endl;
}
