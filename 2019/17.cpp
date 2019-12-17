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

    {
        // Trace the path (could be done manually)
        const std::string directions = "^>v<";
        auto bot = map.find_first_of(directions);
        int row = bot / (width + 1);
        int col = bot % (width + 1);
        int dir = directions.find(getTile(row, col));

        auto getTurn = [&]() {
            const int ldx[] = {-1, 0, 1, 0};
            const int ldy[] = {0, -1, 0, 1};
            if (getTile(row + ldy[dir], col + ldx[dir]) == '#')
            {
                dir = (dir + 3) % 4;
                return 'L';
            }
            const int rdx[] = {1, 0, -1, 0};
            const int rdy[] = {0, 1, 0, -1};
            if (getTile(row + rdy[dir], col + rdx[dir]) == '#')
            {
                dir = (dir + 1) % 4;
                return 'R';
            }
            return 'X';
        };

        auto move = [&]() {
            const int dx[] = {0, 1, 0, -1};
            const int dy[] = {-1, 0, 1, 0};
            int cnt{};
            while (true)
            {
                auto r = row + dy[dir];
                auto c = col + dx[dir];
                if (getTile(r, c) != '#')
                    return cnt;
                ++cnt;
                row = r;
                col = c;
            }
        };

        std::ostringstream oss;
        while (true)
        {
            auto t = getTurn();
            if (t == 'X')
                break;
            oss << t << ",";
            oss << move() << ",";
        }
        std::string trace = oss.str();
        trace.erase(trace.size() - 1);
        MESSAGE(trace);
    }
}
