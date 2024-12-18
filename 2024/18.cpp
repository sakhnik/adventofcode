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
            return p.x + p.y * 100;
        }
    };
};

using MapT = std::unordered_map<Pos, int, Pos::Hash>;

int FindPath(const MapT &map, int time, Pos finish)
{
    auto getMap = [&](Pos p) {
        if (p.x < 0 || p.y < 0 || p.x > finish.x || p.y > finish.y)
            return false;
        auto it = map.find(p);
        return it == map.end() || it->second >= time;
    };

    struct Head
    {
        Pos pos;
        int cost;
    };
    std::queue<Head> q;
    q.push({{0, 0}, 0});
    std::unordered_map<Pos, int, Pos::Hash> cost;
    constexpr const int MAX_PATH = std::numeric_limits<int>::max();
    int goal = MAX_PATH;

    auto go = [&](Pos pos, int c) {
        if (!getMap(pos) || c >= goal)
            return;
        auto it = cost.find(pos);
        if (it == cost.end()) {
            cost[pos] = c;
        } else {
            if (it->second <= c) {
                return;
            }
            it->second = c;
        }
        if (pos == finish) {
            goal = c;
        }
        q.push({pos, c});
    };

    while (!q.empty()) {
        Head head = q.front();
        q.pop();
        go({head.pos.x + 1, head.pos.y}, head.cost + 1);
        go({head.pos.x, head.pos.y + 1}, head.cost + 1);
        go({head.pos.x - 1, head.pos.y}, head.cost + 1);
        go({head.pos.x, head.pos.y - 1}, head.cost + 1);
    }

    return goal == MAX_PATH ? -1 : goal;
}

std::pair<int, std::string> Task(std::istream &&is, int time, Pos finish)
{
    std::vector<Pos> coords;

    MapT map;
    std::string line;
    while (getline(is, line)) {
        Pos p;
        sscanf(line.c_str(), "%d,%d", &p.x, &p.y);
        map[p] = map.size();
        coords.push_back(p);
    }

    int task1 = FindPath(map, time, finish);

    int a = time, b = map.size();
    while (a + 1 < b) {
        int mid = a + (b - a) / 2;
        if (FindPath(map, mid, finish) > 0) {
            a = mid;
        } else {
            b = mid;
        }
    }

    auto p = coords[a];
    return {task1, std::to_string(p.x) + ',' + std::to_string(p.y)};
}

suite s = [] {
    "18"_test = [] {
        const char *const TEST1 = R"(5,4
4,2
4,5
3,0
2,1
6,3
2,4
1,5
0,6
3,3
2,6
5,1
1,2
5,5
2,5
6,5
1,4
0,4
6,4
1,1
6,1
1,0
0,5
1,6
2,0)";
        auto test = Task(std::istringstream{TEST1}, 12, {6, 6});
        expect(22_i == test.first);
        expect(eq(std::string{"6,1"}, test.second));

        auto task = Task(std::ifstream{INPUT}, 1024, {70, 70});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
