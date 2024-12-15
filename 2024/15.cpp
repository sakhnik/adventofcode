#include "../test.hpp"

namespace {

using namespace boost::ut;

struct Pos
{
    int x{}, y{};
    bool operator==(const Pos &o) const = default;
    struct Hash
    {
        size_t operator()(const Pos &r) const
        {
            return std::hash<int>()(r.x + r.y * 100);
        }
    };
};

std::pair<int, int> Task(std::istream &&is)
{
    std::vector<std::string> map;

    std::string line;
    while (getline(is, line)) {
        if (line.empty()) break;
        map.push_back(std::move(line));
    }

    std::string moves;
    while (getline(is, line)) {
        moves += line;
    }

    std::unordered_set<Pos, Pos::Hash> boxes, boxes2;
    Pos robot, robot2;
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == 'O') {
                boxes.insert({x, y});
                boxes2.insert({x+x, y});
            } else if (map[y][x] == '@') {
                robot = {x, y};
                robot2 = {x+x, y};
            }
        }
    }

    auto move = [&](int dx, int dy) {
        Pos n = {robot.x + dx, robot.y + dy};
        if (map[n.y][n.x] == '#')
            return;
        if (boxes.contains(n)) {
            Pos bn = n;
            while (boxes.contains(bn)) {
                bn = {bn.x + dx, bn.y + dy};
                if (map[bn.y][bn.x] == '#')
                    return;
            }
            boxes.erase(n);
            boxes.insert(bn);
        }

        robot = n;
    };

    auto isWall = [&](Pos p) {
        return map[p.y][p.x / 2] == '#';
    };

    auto getBox = [&](Pos p) {
        auto it = boxes2.find(p);
        if (it != boxes2.end()) {
            return p;
        }
        it = boxes2.find({p.x - 1, p.y});
        if (it != boxes2.end()) {
            return *it;
        }
        return Pos{};
    };

    auto pushBoxes = [&](bool dryRun, Pos p, int dx, int dy, auto &&pushBoxes) -> bool {
        if (isWall(p))
            return false;
        auto b = getBox(p);
        if (!b.x)
            return true;
        if (!dy) {
            Pos nb = {b.x + dx, b.y};
            if (!pushBoxes(dryRun, dx < 0 ? nb : Pos{nb.x + 1, nb.y}, dx, dy, pushBoxes))
                return false;
            if (!dryRun) {
                boxes2.erase(b);
                boxes2.insert(nb);
            }
            return true;
        }
        Pos nb = {b.x, b.y + dy};
        if (!pushBoxes(dryRun, nb, dx, dy, pushBoxes)
                || !pushBoxes(dryRun, {b.x + 1, b.y + dy}, dx, dy, pushBoxes))
            return false;
        if (!dryRun) {
            boxes2.erase(b);
            boxes2.insert(nb);
        }
        return true;
    };

    auto move2 = [&](int dx, int dy) {
        Pos n = {robot2.x + dx, robot2.y + dy};
        if (!pushBoxes(true, n, dx, dy, pushBoxes))
            return;
        pushBoxes(false, n, dx, dy, pushBoxes);
        robot2 = n;
    };

    auto print = [&]() {
        for (int y = 0; y < map.size(); ++y) {
            for (int x = 0; x < map[y].size(); ++x) {
                if (robot == Pos{x, y}) {
                    std::cerr << '@';
                } else if (boxes.contains({x, y})) {
                    std::cerr << 'O';
                } else if (map[y][x] == '#') {
                    std::cerr << '#';
                } else {
                    std::cerr << '.';
                }
            }
            std::cerr << std::endl;
        }
        std::cerr << std::endl;
    };

    auto print2 = [&]() {
        for (int y = 0; y < map.size(); ++y) {
            for (int x = 0; x/2 < map[y].size(); ++x) {
                if (robot2 == Pos{x, y}) {
                    std::cerr << '@';
                } else if (boxes2.contains({x, y})) {
                    std::cerr << '[';
                } else if (boxes2.contains({x - 1, y})) {
                    std::cerr << ']';
                } else if (isWall({x, y})) {
                    std::cerr << '#';
                } else {
                    std::cerr << '.';
                }
            }
            std::cerr << std::endl;
        }
        std::cerr << std::endl;
    };

    for (auto m : moves) {
        switch (m) {
        case '^': move(0, -1); move2(0, -1); break;
        case '>': move(1, 0); move2(1, 0); break;
        case 'v': move(0, 1); move2(0, 1); break;
        case '<': move(-1, 0); move2(-1, 0); break;
        }

        //print();
        //print2();
    }

    auto task1 = std::accumulate(boxes.begin(), boxes.end(), 0, [](int a, Pos b) { return a + Pos::Hash{}(b); });
    auto task2 = std::accumulate(boxes2.begin(), boxes2.end(), 0, [](int a, Pos b) { return a + Pos::Hash{}(b); });
    return {task1, task2};
}

suite s = [] {
    "15"_test = [] {
        const char *const TEST1 = R"(########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<)";
        auto test = Task(std::istringstream{TEST1});
        expect(2028_i == test.first);

        const char *const TEST2 = R"(##########
#..O..O.O#
#......O.#
#.OO..O.O#
#..O@..O.#
#O#..O...#
#O..O..O.#
#.OO.O.OO#
#....O...#
##########

<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^
vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v
><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<
<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^
^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><
^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^
>^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^
<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>
^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>
v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^)";
        auto test2 = Task(std::stringstream{TEST2});
        expect(10092_i == test2.first);
        expect(9021_i == test2.second);

        const char *const TEST3 = R"(#######
#...#.#
#.....#
#..OO@#
#..O..#
#.....#
#######

<vv<<^^<<^^)";
        auto test3 = Task(std::stringstream{TEST3});

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
