#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

using namespace boost::ut;

struct Pos
{
    int x, y;

    bool operator==(const Pos &o) const = default;

    struct Hash
    {
        size_t operator()(const Pos &p) const
        {
            size_t s = p.x;
            boost::hash_combine(s, p.y);
            return s;
        }
    };
};

struct PosDir : Pos
{
    int dir;

    bool operator==(const PosDir &o) const = default;

    struct Hash
    {
        size_t operator()(const PosDir &p) const
        {
            size_t s = Pos::Hash{}(p);
            boost::hash_combine(s, p.dir);
            return s;
        }
    };

    PosDir Fw() const
    {
        switch (dir) {
        case 0: return {x, y - 1, dir};
        case 1: return {x + 1, y, dir};
        case 2: return {x, y + 1, dir};
        case 3: return {x - 1, y, dir};
        }
        assert(false);
    }

    PosDir Left() const
    {
        return {x, y, (dir + 3) % 4};
    }

    PosDir Right() const
    {
        return {x, y, (dir + 1) % 4};
    }
};

std::pair<int, int> Task(std::istream &&is)
{
    std::vector<std::string> map;

    std::string line;
    while (getline(is, line)) {
        if (line.empty()) break;
        map.push_back(std::move(line));
    }

    PosDir START = {1, int(map.size() - 2), 1};
    Pos FINISH = {int(map[1].size() - 2), 1};
    int task1 = std::numeric_limits<int>::max();

    struct Cost
    {
        int score = 0;
        std::unordered_set<PosDir, PosDir::Hash> from;
    };
    std::unordered_map<PosDir, Cost, PosDir::Hash> cost;
    struct Head
    {
        PosDir p;
        PosDir prev;
    };
    std::queue<Head> q;
    q.push({START, START});
    cost[START] = {};

    auto go = [&](PosDir np, int s, const PosDir &p) {
        if (map[np.y][np.x] == '#' || s > task1)
            return;
        auto it = cost.find(np);
        if (it != cost.end()) {
            auto &c = it->second;
            if (c.score < s) {
                return;
            } else if (c.score == s) {
                c.from.insert(p);
            } else {
                c.from = {p};
                c.score = s;
            }
        } else {
            cost[np] = {s, {p}};
        }
        if (FINISH == np) {
            task1 = s;
        }
        q.push({np, p});
    };

    while (!q.empty()) {
        Head head = q.front();
        q.pop();
        const auto &c = cost[head.p];
        go(head.p.Fw(), c.score + 1, head.p);
        go(head.p.Left(), c.score + 1000, head.prev);
        go(head.p.Right(), c.score + 1000, head.prev);
    }

    std::unordered_set<Pos, Pos::Hash> visited;
    auto dfs = [&](PosDir p, auto &&dfs) -> void {
        visited.insert(p);
        for (const auto &p1 : cost[p].from) {
            dfs(p1, dfs);
        }
    };
    for (int i = 0; i < 4; ++i)
        dfs(PosDir{FINISH, i}, dfs);

    //for (int y = 0; y < map.size(); ++y) {
    //    for (int x = 0; x < map[y].size(); ++x) {
    //        if (visited.contains(Pos{x, y})) {
    //            std::cerr << "O";
    //        } else {
    //            std::cerr << map[y][x];
    //        }
    //    }
    //    std::cerr << std::endl;
    //}

    return {task1, visited.size()};
}

suite s = [] {
    "16"_test = [] {
//        const char *const TEST1 = R"(###############
//#.......#....E#
//#.#.###.#.###.#
//#.....#.#...#.#
//#.###.#####.#.#
//#.#.#.......#.#
//#.#.#####.###.#
//#...........#.#
//###.#.#####.#.#
//#...#.....#.#.#
//#.#.#.###.#.#.#
//#.....#...#.#.#
//#.###.#.#.#.#.#
//#S..#.....#...#
//###############)";
//        auto test = Task(std::istringstream{TEST1});
//        expect(7036_i == test.first);
//        expect(45_i == test.second);
//
//        const char *const TEST2 = R"(#################
//#...#...#...#..E#
//#.#.#.#.#.#.#.#.#
//#.#.#.#...#...#.#
//#.#.#.#.###.#.#.#
//#...#.#.#.....#.#
//#.#.#.#.#.#####.#
//#.#...#.#.#.....#
//#.#.#####.#.###.#
//#.#.#.......#...#
//#.#.###.#####.###
//#.#.#...#.....#.#
//#.#.#.#####.###.#
//#.#.#.........#.#
//#.#.#.#########.#
//#S#.............#
//#################)";
//        auto test2 = Task(std::istringstream{TEST2});
//        expect(11048_i == test2.first);
//        expect(64_i == test2.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
