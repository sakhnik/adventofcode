#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>
#include <set>


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

class Bot
{
public:
    Bot(IntCodeB prog)
        : _prog{prog}
    {
    }

    void Paint(bool is_white)
    {
        if (is_white)
        {
            _whites.insert({0,0});
        }

        int x{}, y{};
        int dir{};
        int dx[] = {0, 1, 0, -1};
        int dy[] = {-1, 0, 1, 0};

        while (true)
        {
            _prog.Advance(0);
            if (_prog.GetState() == _prog.S_HALT)
            {
                break;
            }
            REQUIRE(_prog.GetState() == _prog.S_INPUT);
            auto it = _whites.find({x, y});
            is_white = it != _whites.end();

            auto r = _prog.Advance(is_white);
            REQUIRE(_prog.GetState() == _prog.S_OUTPUT);
            if (r != is_white)
            {
                // Need to repaint
                _repaints.insert({x,y});
                if (r)
                {
                    _whites.insert({x,y});
                }
                else
                {
                    _whites.erase(it);
                }
            }

            r = _prog.Advance(0);
            REQUIRE(_prog.GetState() == _prog.S_OUTPUT);

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
    }

    int CountRepaints() const
    {
        return _repaints.size();
    }

    std::string Print() const
    {
        Pos tl{}, br{};
        for (auto p : _whites)
        {
            tl.x = std::min(tl.x, p.x);
            tl.y = std::min(tl.y, p.y);
            br.x = std::max(br.x, p.x);
            br.y = std::max(br.y, p.y);
        }

        std::string res;
        for (int y = tl.y; y <= br.y; ++y)
        {
            for (int x = tl.x; x <= br.x; ++x)
            {
                res.push_back(_whites.find({x,y}) != _whites.end() ? '#' : ' ');
            }
            res.push_back('\n');
        }
        return res;
    }

private:
    IntCodeB _prog;
    std::set<Pos> _repaints;
    std::set<Pos> _whites;
};


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCodeB prog{ifs};

    Bot b1{prog};
    b1.Paint(false);

    MESSAGE(b1.CountRepaints());

    Bot b2{prog};
    b2.Paint(true);

    MESSAGE("\n" << b2.Print());
}
