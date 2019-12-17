#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>
#include <sstream>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCodeB prog{ifs};

    std::ostringstream oss;
    {
        IntCodeB p1{prog};
        while (p1.GetState() != p1.S_HALT)
        {
            auto r = p1.Advance(0);
            switch (p1.GetState())
            {
            case p1.S_RUN:
            case p1.S_HALT:
                break;
            case p1.S_INPUT:
                REQUIRE(!"Not supported");
                break;
            case p1.S_OUTPUT:
                oss << char(r);
                break;
            }
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

        std::vector<std::string> commands;
        while (true)
        {
            auto t = getTurn();
            if (t == 'X')
                break;
            commands.emplace_back(std::string{t});
            commands.emplace_back(std::to_string(move()));
        }

        for (auto s : commands)
            std::cout << s << ",";
        std::cout << std::endl;

        // L,8,R,10,L,8,R,8,L,12,R,8,R,8,L,8,R,10,L,8,R,8,L,8,R,6,R,6,R,10,L,8,L,8,R,6,R,6,R,10,L,8,L,8,R,10,L,8,R,8,L,12,R,8,R,8,L,8,R,6,R,6,R,10,L,8,L,12,R,8,R,8,L,12,R,8,R,8
        //
        // A,L,12,R,8,R,8,A,L,8,R,6,R,6,R,10,L,8,L,8,R,6,R,6,R,10,L,8,A,L,12,R,8,R,8,L,8,R,6,R,6,R,10,L,8,L,12,R,8,R,8,L,12,R,8,R,8
        // A = L,8,R,10,L,8,R,8
        //
        // A,B,A,L,8,R,6,R,6,R,10,L,8,L,8,R,6,R,6,R,10,L,8,A,B,L,8,R,6,R,6,R,10,L,8,B,B
        // A = L,8,R,10,L,8,R,8
        // B = L,12,R,8,R,8
        //
        // A,B,A,C,C,A,B,C,B,B
        // A = L,8,R,10,L,8,R,8
        // B = L,12,R,8,R,8
        // C = L,8,R,6,R,6,R,10,L,8
    }

    {
        const char *commands =
            "A,B,A,C,C,A,B,C,B,B\n"
            "L,8,R,10,L,8,R,8\n"
            "L,12,R,8,R,8\n"
            "L,8,R,6,R,6,R,10,L,8\n"
            "n\n";

        IntCodeB p2{prog};
        p2.SetMemory(0, 2);
        BigIntT input = 0;

        while (p2.GetState() != p2.S_HALT)
        {
            auto r = p2.Advance(input);
            switch (p2.GetState())
            {
            case p2.S_RUN:
            case p2.S_HALT:
                break;
            case p2.S_INPUT:
                input = *commands++;
                break;
            case p2.S_OUTPUT:
                if (r < 256)
                {
                    // Prompts, map output
                    std::cout << char(r);
                }
                else
                {
                    // The score
                    MESSAGE(r);
                }
                break;
            }
        }
    }
}
