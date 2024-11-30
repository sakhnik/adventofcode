#include <unordered_map>
#include <fstream>
#include <boost/functional/hash.hpp>
#include "../test.hpp"

namespace {

using namespace boost::ut;

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
        expect(false);
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

suite s = [] {
    "22"_test = [] {
        {
            std::istringstream iss("..#\n#..\n...");
            auto grid = Parse(iss);
            expect(grid.size() == 9_u);
            expect(grid[CoordT(-1, 0)] == INFECTED);
            expect(grid[CoordT(1, -1)] == INFECTED);

            expect(Count(7, grid) == 5_u);
            expect(Count(70, grid) == 41_u);
            expect(Count(10000, grid) == 5587_u);
        }

        {
            std::istringstream iss("..#\n#..\n...");
            auto grid = Parse(iss);

            expect(Count2(100, grid) == 26_u);
            expect(Count2(10000000, grid) == 2511944_u);
        }

        std::ifstream ifs(INPUT);
        auto grid = Parse(ifs);
        Printer::Print(__FILE__, "1", Count(10000, grid));
        if (Printer::EnableAll())
        {
            Printer::Print(__FILE__, "2", Count2(10000000, grid));
        }
    };
};

} //namespace;
