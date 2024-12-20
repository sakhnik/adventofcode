#include "../test.hpp"

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
            return p.y * 1000 + p.x;
        }
    };
};

std::pair<int, int64_t> Task(std::istream &&is, int threshold)
{
    Pos START{}, FINISH{};

    std::vector<std::string> map;
    std::string line;
    int y = 0;
    while (getline(is, line)) {
        auto x = line.find('S');
        if (x != line.npos)
            START = {int(x), y};
        x = line.find('E');
        if (x != line.npos)
            FINISH = {int(x), y};
        map.push_back(std::move(line));
        ++y;
    }

    struct Cost
    {
        int dist = std::numeric_limits<int>::max();
        Pos from;
    };
    std::unordered_map<Pos, Cost, Pos::Hash> cost;
    cost[START] = {0, START};
    std::queue<Pos> q;
    q.push(START);

    auto go = [&](Pos p, int dist, Pos from) {
        if (map[p.y][p.x] == '#')
            return;
        auto c = cost[p];
        if (c.dist <= dist)
            return;
        cost[p] = {dist, from};
        q.push(p);
    };

    while (!q.empty()) {
        Pos p = q.front();
        q.pop();
        auto c = cost[p];
        go({p.x + 1, p.y}, c.dist + 1, p);
        go({p.x, p.y + 1}, c.dist + 1, p);
        go({p.x - 1, p.y}, c.dist + 1, p);
        go({p.x, p.y - 1}, c.dist + 1, p);
    }

    std::vector<Pos> track{FINISH};
    while (track.back() != START) {
        track.push_back(cost[track.back()].from);
    }

    int task1{}, task2{};
    for (int i = 0; i < track.size(); ++i) {
        Pos a = track[i];
        for (int j = i + threshold; j < track.size(); ++j) {
            Pos b = track[j];
            auto d = std::abs(a.x - b.x) + std::abs(a.y - b.y);
            if (j - i - d < threshold)
                continue;
            if (d <= 2)
                ++task1;
            if (d <= 20)
                ++task2;
        }
    }

    return {task1, task2};
}

suite s = [] {
    "20"_test = [] {
        const char *const TEST1 = R"(###############
#...#...#.....#
#.#.#.#.#.###.#
#S#...#.#.#...#
#######.#.#.###
#######.#.#...#
#######.#.###.#
###..E#...#...#
###.#######.###
#...###...#...#
#.#####.#.###.#
#.#...#.#.#...#
#.#.#.#.#.#.###
#...#...#...###
###############)";
        auto test = Task(std::istringstream{TEST1}, 20);
        expect(5_i == test.first);

        auto test2 = Task(std::istringstream{TEST1}, 74);
        expect(7_i == test2.second);

        auto task = Task(std::ifstream{INPUT}, 100);
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
