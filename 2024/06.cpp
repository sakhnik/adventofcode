#include "../test.hpp"

namespace {

using namespace boost::ut;

struct Pos
{
    int x = 0, y = 0;

    Pos Next(int dir)
    {
        switch (dir) {
        case 0: return {x, y - 1};
        case 1: return {x + 1, y};
        case 2: return {x, y + 1};
        case 3:
        default:
                return {x - 1, y};
        }
    }

    bool operator==(Pos o) const { return x == o.x && y == o.y; }
};

static constexpr auto pos_hash = [](const Pos &p) {
    return std::hash<int>()((p.x << 16) | p.y);
};

using MapT = std::unordered_set<Pos, decltype(pos_hash)>;

// Trace guard movement.
std::pair<bool, MapT> Trace(Pos guard, Pos limit, const MapT &map)
{
    MapT trace;
    int dir = 0;

    auto getKey = [&]() {
        uint32_t key = dir;
        key = (key << 8) + guard.x;
        key = (key << 8) + guard.y;
        return key;
    };
    std::unordered_set<uint32_t> config;

    while (guard.x >= 0 && guard.x < limit.x && guard.y >= 0 && guard.y < limit.y) {
        auto key = getKey();
        if (config.contains(key)) {
            return {true, trace};
        }
        config.insert(key);
        trace.insert(guard);
        auto next = guard.Next(dir);
        auto it = map.find(next);
        if (it != map.end()) {
            dir = (dir + 1) % 4;
        } else {
            guard = next;
        }
    }
    return {false, trace};
}

std::pair<int, int> Task(std::istream &&is)
{
    MapT map;
    Pos guard;
    int maxX{}, maxY{};

    std::string line;
    int y{};
    while (getline(is, line)) {
        maxX = line.size();
        for (int x = 0; x < line.size(); ++x) {
            if (line[x] == '#') {
                map.insert({x, y});
            } else if (line[x] == '^') {
                guard = {x, y};
            }
        }
        ++y;
    }
    maxY = y;

    auto [_, trace] = Trace(guard, {maxX, maxY}, map);
    auto task1 = trace.size();

    int task2{};
    for (auto o : trace) {
        if (o == guard) continue;
        map.insert(o);
        auto [loop, _] = Trace(guard, {maxX, maxY}, map);
        task2 += loop;
        map.erase(o);
    }

    return {task1, task2};
}

suite s = [] {
    "06"_test = [] {
        const char *const TEST1 = R"(....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...)";
        auto test = Task(std::istringstream{TEST1});
        expect(41_i == test.first);
        expect(6_i == test.second);

        if (Printer::EnableAll()) {
            auto task = Task(std::ifstream{INPUT});
            Printer::Print(__FILE__, "1", task.first);
            Printer::Print(__FILE__, "2", task.second);
        }
    };
};

} //namespace;
