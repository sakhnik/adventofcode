#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>
#include <set>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCode prog{ifs};

    struct Pos
    {
        int x,y;
        bool operator<(const Pos &o) const
        {
            if (x < o.x) return true;
            if (x > o.x) return false;
            return y < o.y;
        }
    };

    std::set<Pos> repaints;
    std::set<Pos> whites;
    int x{}, y{};
    int dir{};
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    while (true)
    {
        prog.Advance(0);
        if (prog.GetState() == prog.S_HALT)
        {
            break;
        }
        REQUIRE(prog.GetState() == prog.S_INPUT);
        auto it = whites.find({x, y});
        bool is_white = it != whites.end();

        auto r = prog.Advance(is_white);
        REQUIRE(prog.GetState() == prog.S_OUTPUT);
        if (r != is_white)
        {
            // Need to repaint
            repaints.insert({x,y});
            if (r)
            {
                whites.insert({x,y});
            }
            else
            {
                whites.erase(it);
            }
        }

        r = prog.Advance(0);
        REQUIRE(prog.GetState() == prog.S_OUTPUT);

        if (r)
        {
            dir = (dir + 1) % 4;
        }
        else
        {
            dir = (4 + dir - 1) % 4;
        }
        x += dx[dir];
        y += dy[dir];
    }

    MESSAGE(repaints.size());
}
