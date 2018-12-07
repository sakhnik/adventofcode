#include <doctest/doctest.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <boost/functional/hash.hpp>

namespace {

typedef std::pair<int, int> CoordT;

struct MyHash
{
    typedef CoordT argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept
    {
        result_type seed{0};
        boost::hash_combine(seed, s.first);
        boost::hash_combine(seed, s.second);
        return seed;
    }
};


enum State
{
    CLEAN = 0,
    WEAKENED,
    INFECTED,
    FLAGGED,
};

typedef std::unordered_map<CoordT, State, MyHash> GridT;

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
            grid[CoordT(x - centre, y - centre)] = line[x] == '#' ? INFECTED : CLEAN;
        }
        ++y;
    }

    return grid;
}

enum Direction
{
    UP = 0,
    RIGHT,
    DOWN,
    LEFT,

    DIRECTION_COUNT
};

Direction TurnLeft(Direction d)
{
    return static_cast<Direction>((d + DIRECTION_COUNT - 1) % DIRECTION_COUNT);
}

Direction TurnRight(Direction d)
{
    return static_cast<Direction>((d + 1) % DIRECTION_COUNT);
}

Direction TurnBack(Direction d)
{
    return static_cast<Direction>((d + 2) % DIRECTION_COUNT);
}

void Move(Direction dir, CoordT &pos)
{
    switch (dir)
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

unsigned Count(unsigned iterations, GridT grid)
{
    unsigned count{0};
    CoordT pos{0, 0};
    Direction direction{UP};

    while (iterations--)
    {
        auto &state = grid[pos];
        switch (state)
        {
        case CLEAN:
            direction = TurnLeft(direction);
            state = INFECTED;
            ++count;
            break;
        case INFECTED:
            direction = TurnRight(direction);
            state = CLEAN;
            break;
        default:
            break;
        }

        Move(direction, pos);
    }

    return count;
}

unsigned Count2(unsigned iterations, GridT grid)
{
    unsigned count{0};
    CoordT pos{0, 0};
    Direction direction{UP};

    while (iterations--)
    {
        auto &state = grid[pos];
        switch (state)
        {
        case CLEAN:
            direction = TurnLeft(direction);
            state = WEAKENED;
            break;
        case WEAKENED:
            state = INFECTED;
            ++count;
            break;
        case INFECTED:
            direction = TurnRight(direction);
            state = FLAGGED;
            break;
        case FLAGGED:
            direction = TurnBack(direction);
            state = CLEAN;
            break;
        default:
            break;
        }

        Move(direction, pos);
    }

    return count;
}

} //namespace;

TEST_CASE("1")
{
    std::istringstream iss("..#\n#..\n...");
    auto grid = Parse(iss);
    REQUIRE(grid.size() == 9);
    REQUIRE(grid[CoordT(-1,0)] == INFECTED);
    REQUIRE(grid[CoordT(1,-1)] == INFECTED);

    CHECK(Count(7, grid) == 5);
    CHECK(Count(70, grid) == 41);
    CHECK(Count(10000, grid) == 5587);
}

TEST_CASE("2")
{
    std::istringstream iss("..#\n#..\n...");
    auto grid = Parse(iss);

    CHECK(Count2(100, grid) == 26);
    CHECK(Count2(10000000, grid) == 2511944);
}

TEST_CASE(TEST_NAME)
{
    std::ifstream ifs(INPUT);
    auto grid = Parse(ifs);
    std::cout << Count(10000, grid) << std::endl;
    std::cout << Count2(10000000, grid) << std::endl;
}
