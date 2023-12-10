#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

class Pipe
{
public:
    Pipe(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
            maze.push_back(line);
        for (int row = 0; row < maze.size(); ++row)
        {
            auto col = maze[row].find('S');
            if (col != maze[row].npos)
            {
                start = {row, static_cast<int>(col)};
                break;
            }
        }
    }

    int Task1()
    {
        for (int dir = 0; dir < 4; ++dir)
        {
            auto length = TraceLoop(dir);
            if (length > 0)
                return length;
        }
        return -1;
    }

private:
    std::vector<std::string> maze;
    struct Pos
    {
        int row{}, col{};
        int dir{};
    };
    Pos start;
    static constexpr std::string PIPES = "|-LF7J";
    // Assumed directions: ↑ = 0, → = 1, ↓ = 2, ← = 3
    static constexpr const std::string ENTRIES[6] = {{0, 2}, {1, 3}, {2, 3}, {3, 0}, {0, 1}, {1, 2}};
    static constexpr const std::string EXITS[6] = {{0, 2}, {1, 3}, {1, 0}, {2, 1}, {3, 2}, {0, 3}};
    static constexpr const std::string MOVE_ROW = {-1, 0, 1, 0}, MOVE_COL = {0, 1, 0, -1};

    bool CheckPos(const Pos &pos) const
    {
        if (pos.row < 0 || pos.row >= maze.size())
            return false;
        if (pos.col < 0 || pos.col >= maze[0].size())
            return false;
        return true;
    }

    bool Follow(Pos &pos, char joint) const
    {
        auto pipe_idx = PIPES.find(joint);
        if (pipe_idx == PIPES.npos)
            return false;
        auto entries = ENTRIES[pipe_idx];
        auto entry_idx = entries.find(pos.dir);
        if (entry_idx == entries.npos)
            return false;
        auto exits = EXITS[pipe_idx];
        pos.dir = exits[entry_idx];
        Move(pos);
        return CheckPos(pos);
    }

    static void Move(Pos &pos)
    {
        pos.row += MOVE_ROW[pos.dir];
        pos.col += MOVE_COL[pos.dir];
    }

    int TraceLoop(int dir) const
    {
        int length{};
        Pos pos = start;
        pos.dir = dir;
        Move(pos);
        if (!CheckPos(pos))
            return -1;
        while (Follow(pos, maze[pos.row][pos.col]))
        {
            ++length;
            if (maze[pos.row][pos.col] == 'S')
                return (length + 1) / 2;
        }
        return -1;
    }
};

suite s = [] {
    "2023-10"_test = [] {
        const char *const TEST1 = R"(-L|F7
7S-7|
L|7||
-L-J|
L|-JF
)";
        const char *const TEST2 = R"(7-F7-
.FJ|7
SJLL7
|F--J
LJ.LJ
)";
        Pipe test1{std::istringstream{TEST1}};
        expect(4_i == test1.Task1());
        Pipe test2{std::istringstream{TEST2}};
        expect(8_i == test2.Task1());

        Pipe pipe{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", pipe.Task1());
        //Printer::Print(__FILE__, "2", mirage.Task2());
    };
};

} //namespace;
