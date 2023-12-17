#include "../test.hpp"
#include <fstream>
#include <boost/functional/hash.hpp>

namespace {

using namespace boost::ut;

struct Map
{
    std::vector<std::string> map;

    Map(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
            map.push_back(line);
    }

    int Task1() const
    {
        const auto height = map.size();
        const auto width = map[0].size();

        struct Info
        {
            int row{}, col{};
            int dir{};
            int streak{};

            bool operator==(const Info &o) const = default;
        };
        auto infoHash = [](const Info &info) {
            size_t seed{};
            boost::hash_combine(seed, info.row);
            boost::hash_combine(seed, info.col);
            boost::hash_combine(seed, info.dir);
            boost::hash_combine(seed, info.streak);
            return seed;
        };

        std::unordered_set<Info, decltype(infoHash)> visited(0, infoHash);

        // Dijkstra search
        struct Edge : Info
        {
            int dist{};

            bool operator<(const Edge &o) const { return dist > o.dist; }
        };

        std::priority_queue<Edge> q;
        q.push({{0, 0, 0, 0}, 0});
        while (!q.empty())
        {
            auto e = q.top();
            q.pop();
            if (e.row == height - 1 && e.col == width - 1)
                return e.dist;

            if (visited.count(e))
                continue;
            visited.insert(e);

            auto tryGo = [&](int row, int col, int new_dir) {
                if (row < 0 || row >= height) return;
                if (col < 0 || col >= width) return;
                if (e.dir == new_dir && e.streak >= 3) return;
                if (new_dir == (e.dir + 2) % 4) return;

                auto new_dist = e.dist + (map[row][col] - '0');
                Edge edge{{row, col, new_dir, e.dir == new_dir ? e.streak + 1 : 1}, new_dist};
                q.push(edge);
            };

            tryGo(e.row, e.col + 1, 0);
            tryGo(e.row + 1, e.col, 1);
            tryGo(e.row, e.col - 1, 2);
            tryGo(e.row - 1, e.col, 3);
        }

        return -1;
    }
};

suite s = [] {
    "2023-17"_test = [] {
        const char *const TEST1 = R"(2413432311323
3215453535623
3255245654254
3446585845452
4546657867536
1438598798454
4457876987766
3637877979653
4654967986887
4564679986453
1224686865563
2546548887735
4322674655533
)";
        Map test1{std::istringstream{TEST1}};
        expect(102_i == test1.Task1());
        //expect(51_i == test1.Task2());

        Map map{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", map.Task1());
        //Printer::Print(__FILE__, "2", map.Task2());
    };
};

} //namespace;
