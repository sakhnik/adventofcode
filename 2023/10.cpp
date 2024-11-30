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
            {
                // Remember the found loop for task 2
                loop_dir = dir;
                return length / 2;
            }
        }
        return -1;
    }

    int Task2()
    {
        static constexpr auto pos_hash = [](const Pos &p) {
            return std::hash<int>()((p.row << 16) | p.col);
        };
        std::unordered_set<Pos, decltype(pos_hash)> visited{0, pos_hash};
        std::queue<Pos> to_visit;
        bool hit_border = false;

        auto enqueue = [&](Pos pos) {
            if (!CheckPos(pos))
            {
                hit_border = true;
                return;
            }
            if (!visited.count(pos))
                to_visit.push(pos);
        };

        auto loop_length = TraceLoop(loop_dir, [&](Pos pos) {
            visited.insert(pos);
            // Let's look aside into one direction all the time
            // TODO: There's a bug somewhere that produces a wrong output when looking into the other direction.
            // pos.dir = (pos.dir + 1) % 4;
            pos.dir = (pos.dir + 3) % 4;
            Move(pos);
            enqueue(pos);
        });
        assert(visited.size() == loop_length);

        while (!to_visit.empty())
        {
            auto pos = to_visit.front();
            to_visit.pop();
            if (visited.count(pos))
                continue;
            visited.insert(pos);
            enqueue({pos.row - 1, pos.col});
            enqueue({pos.row + 1, pos.col});
            enqueue({pos.row, pos.col - 1});
            enqueue({pos.row, pos.col + 1});
        }

        if (hit_border)
            return maze.size() * maze[0].size() - visited.size();
        std::cout << 2 << std::endl;
        return visited.size() - loop_length;
    }

private:
    std::vector<std::string> maze;
    struct Pos
    {
        int row{}, col{};
        int dir{};
        bool operator==(const Pos &o) const { return row == o.row && col == o.col ;}
    };
    Pos start;
    int loop_dir{};
    static constexpr const std::string_view PIPES = "|-LF7J";
    // Assumed directions: ↑ = 0, → = 1, ↓ = 2, ← = 3
    static constexpr const int ENTRIES[6][2] = {{0, 2}, {1, 3}, {2, 3}, {3, 0}, {0, 1}, {1, 2}};
    static constexpr const int EXITS[6][2] = {{0, 2}, {1, 3}, {1, 0}, {2, 1}, {3, 2}, {0, 3}};
    static constexpr const int MOVE_ROW[4] = {-1, 0, 1, 0}, MOVE_COL[4] = {0, 1, 0, -1};

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
        auto entry_idx = -1;
	if (entries[0] == pos.dir)
	    entry_idx = 0;
	else if (entries[1] == pos.dir)
	    entry_idx = 1;
	else
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

    int TraceLoop(int dir, std::function<void(Pos)> on_position = [](Pos){}) const
    {
        int length{1};
        Pos pos = start;
        pos.dir = dir;
        Move(pos);
        if (!CheckPos(pos))
            return -1;
        on_position(pos);
        while (Follow(pos, maze[pos.row][pos.col]))
        {
            ++length;
            on_position(pos);
            if (maze[pos.row][pos.col] == 'S')
                return length;
        }
        return -1;
    }
};

suite s = [] {
    "10"_test = [] {
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
        expect(1_i == test1.Task2());
        Pipe test2{std::istringstream{TEST2}};
        expect(8_i == test2.Task1());

        const char *const TEST3 = R"(.....
.S-7.
.|.|.
.L-J.
.....
)";

        Pipe test3{std::istringstream{TEST3}};
        test3.Task1();
        expect(1_i == test3.Task2());

        const char *const TEST4 = R"(...........
.S-------7.
.|F-----7|.
.||.....||.
.||.....||.
.|L-7.F-J|.
.|..|.|..|.
.L--J.L--J.
...........
)";
        Pipe test4{std::istringstream{TEST4}};
        test4.Task1();
        expect(4_i == test4.Task2());

        const char *const TEST5 = R"(.F----7F7F7F7F-7....
.|F--7||||||||FJ....
.||.FJ||||||||L7....
FJL7L7LJLJ||LJ.L-7..
L--J.L7...LJS7F-7L7.
....F-J..F7FJ|L7L7L7
....L7.F7||L7|.L7L7|
.....|FJLJ|FJ|F7|.LJ
....FJL-7.||.||||...
....L---J.LJ.LJLJ...
)";
        Pipe test5{std::istringstream{TEST5}};
        test5.Task1();
        expect(8_i == test5.Task2());

        const char *const TEST6 = R"(FF7FSF7F7F7F7F7F---7
L|LJ||||||||||||F--J
FL-7LJLJ||||||LJL-77
F--JF--7||LJLJ7F7FJ-
L---JF-JLJ.||-FJLJJ7
|F|F-JF---7F7-L7L|7|
|FFJF7L7F-JF7|JL---7
7-L-JL7||F7|L7F-7F7|
L.L7LFJ|||||FJL7||LJ
L7JLJL-JLJLJL--JLJ.L
)";
        Pipe test6{std::istringstream{TEST6}};
        test6.Task1();
        expect(10_i == test6.Task2());

        Pipe pipe{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", pipe.Task1());
        Printer::Print(__FILE__, "2", pipe.Task2());
    };
};

} //namespace;
