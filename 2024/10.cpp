#include "../test.hpp"

namespace {

using namespace boost::ut;

struct Pos
{
    int x = 0, y = 0;

    bool operator==(Pos o) const { return x == o.x && y == o.y; }
};

static constexpr auto pos_hash = [](const Pos &p) {
    return std::hash<int>()((p.x << 16) | p.y);
};

std::pair<int, int> Task(std::istream &&is)
{
    std::vector<std::string> map;
    std::string line;
    while (getline(is, line)) {
        map.push_back(std::move(line));
    }

    auto getMap = [&](Pos p) {
        return map[p.y][p.x];
    };
    auto isValid = [&](Pos p) {
        return p.x >= 0 && p.y >= 0 && p.y < map.size() && p.x < map[0].size();
    };

    int task1{};

    using SetT = std::unordered_set<Pos, decltype(pos_hash)>;

    auto dfs = [&](Pos p, char height, SetT &visited, auto &&dfs) {
        if (!isValid(p) || getMap(p) != height || visited.contains(p))
            return;
        visited.insert(p);
        if (height == '9') {
            ++task1;
            return;
        }
        dfs({p.x, p.y - 1}, height + 1, visited, dfs);
        dfs({p.x, p.y + 1}, height + 1, visited, dfs);
        dfs({p.x - 1, p.y}, height + 1, visited, dfs);
        dfs({p.x + 1, p.y}, height + 1, visited, dfs);
    };

    int task2{};
    auto bfs = [&](Pos start) {
        using MapT = std::unordered_map<Pos, int, decltype(pos_hash)>;
        MapT counts;
        counts[start] = 1;

        std::queue<Pos> q;
        q.push(start);

        auto go = [&](Pos p, char height, int prevCounts) {
            if (!isValid(p) || getMap(p) != height)
                return;
            if (height == '9')
                task2 += prevCounts;
            if (!counts.contains(p))
                q.push(p);
            counts[p] += prevCounts;
            return;
        };

        while (!q.empty()) {
            auto p = q.front();
            q.pop();
            auto height = getMap(p);
            auto curCounts = counts[p];
            go({p.x, p.y + 1}, height + 1, curCounts);
            go({p.x, p.y - 1}, height + 1, curCounts);
            go({p.x + 1, p.y}, height + 1, curCounts);
            go({p.x - 1, p.y}, height + 1, curCounts);
        }
    };

    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[0].size(); ++x) {
            if (map[y][x] == '0') {
                SetT visited;
                dfs({x, y}, '0', visited, dfs);
                bfs({x, y});
            }
        }
    }

    return {task1, task2};
}

suite s = [] {
    "10"_test = [] {
        const char *const TEST1 = R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732)";
        auto test = Task(std::istringstream{TEST1});
        expect(36_i == test.first);
        expect(81_i == test.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
