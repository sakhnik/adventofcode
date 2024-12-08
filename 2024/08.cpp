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
    std::unordered_map<char, std::vector<Pos>> antennas;
    Pos edge;

    int y{};
    std::string line;
    while (getline(is, line)) {
        edge.x = line.size();
        for (int x = 0; x < line.size(); ++x) {
            if (line[x] != '.') {
                antennas[line[x]].push_back({x, y});
            }
        }
        ++y;
    }
    edge.y = y;

    std::unordered_set<Pos, decltype(pos_hash)> nodes1, nodes2;
    auto addNode = [&](Pos p, auto &nodes) {
        if (p.x < 0 || p.y < 0 || p.x >= edge.x || p.y >= edge.y)
            return;
        nodes.insert(p);
    };

    for (const auto &[a, pos] : antennas) {
        for (auto a : pos) {
            for (auto b : pos) {
                if (a != b) {
                    int dx = b.x - a.x;
                    int dy = b.y - a.y;
                    for (int i = 0; i < edge.x; ++i) {
                        Pos n = {b.x + i * dx, b.y + i * dy};
                        if (i == 1)
                            addNode(n, nodes1);
                        addNode(n, nodes2);
                    }
                }
            }
        }
    }

    return {nodes1.size(), nodes2.size()};
}

suite s = [] {
    "08"_test = [] {
        const char *const TEST1 = R"(............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............)";
        auto test = Task(std::istringstream{TEST1});
        expect(14_i == test.first);
        expect(34_i == test.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
