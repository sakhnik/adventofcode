#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Hiking
{
    std::vector<std::string> map;

    Hiking(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
            map.push_back(line);
    }

    int Walk(bool ignore_slopes)
    {
        using PosT = std::array<int, 2>;

        PosT start{0, 1};
        PosT target{static_cast<int>(map.size()) - 1, static_cast<int>(map[0].size()) - 2};
        static constexpr auto posHash = [](const PosT &p) {
            return std::hash<int>()((p[0] << 16) | p[1]);
        };
        std::unordered_set<PosT, decltype(posHash)> visited(0, posHash);

        // Position change for the direction
        const PosT dp[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        int longest_walk{0};

        auto getPlace = [&](const PosT &pos) -> char {
            if (pos[0] < 0 || pos[0] >= map.size()) return '#';
            if (pos[1] < 0 || pos[1] >= map[0].size()) return '#';
            return map[pos[0]][pos[1]];
        };

        auto follow = [&](const PosT &pos, int dir) -> PosT {
            return { pos[0] + dp[dir][0], pos[1] + dp[dir][1]};
        };

        auto getAllDirections = [&](const PosT &pos) -> std::string {
            char cur_place = getPlace(pos);
            if (!ignore_slopes)
            {
                auto forced_dir = std::string{">v<^"}.find_first_of(cur_place);
                if (forced_dir != std::string::npos)
                    return std::string{char(forced_dir)};
            }
            std::string res;
            for (int i : {0, 1, 2, 3})
                if (getPlace(follow(pos, i)) != '#')
                    res.push_back(i);
            return res;
        };

        using NeighboursT = std::unordered_map<PosT, int, decltype(posHash)>;
        std::unordered_map<PosT, NeighboursT, decltype(posHash)> graph;
        for (int row = 0; row < map.size(); ++row)
        {
            for (int col = 0; col < map.size(); ++col)
            {
                if (map[row][col] == '#')
                    continue;
                auto dirs = getAllDirections({row, col});
                for (auto dir : dirs)
                    graph[{row, col}][follow({row, col}, dir)] = 1;
            }
        }

        for (auto it = graph.begin(); it != graph.end(); )
        {
            auto &neighbours = it->second;
            if (neighbours.size() == 2)
            {
                auto it2 = neighbours.begin();
                auto left = *it2++;
                auto right = *it2++;
                auto &left_neighbour = graph[left.first];
                auto &right_neighbour = graph[right.first];
                left_neighbour.erase(it->first);
                right_neighbour.erase(it->first);
                auto it3 = left_neighbour.find(right.first);
                int dist = it3 != left_neighbour.end() ? it3->second : 0;
                dist = std::max(dist, neighbours[left.first] + neighbours[right.first]);
                left_neighbour[right.first] = dist;
                right_neighbour[left.first] = dist;
                it = graph.erase(it);
            }
            else
                ++it;
        }

        //std::cerr << graph.size() << std::endl;

        auto trace = [&](PosT pos, int dist, auto &&trace) {
            if (pos == target)
            {
                longest_walk = std::max(longest_walk, dist);
                //std::cerr << longest_walk << std::endl;
                return;
            }
            if (visited.count(pos))
                return;
            visited.insert(pos);

            for (auto next_dir : graph[pos])
                trace(next_dir.first, dist + next_dir.second, trace);

            visited.erase(pos);
        };
        trace(start, 0, trace);

        return longest_walk;
    }

    int Task1()
    {
        return Walk(false);
    }

    int Task2()
    {
        return Walk(true);
    }
};

suite s = [] {
    "2023-23"_test = [] {
        const char *const TEST1 = R"(#.#####################
#.......#########...###
#######.#########.#.###
###.....#.>.>.###.#.###
###v#####.#v#.###.#.###
###.>...#.#.#.....#...#
###v###.#.#.#########.#
###...#.#.#.......#...#
#####.#.#.#######.#.###
#.....#.#.#.......#...#
#.#####.#.#.#########v#
#.#...#...#...###...>.#
#.#.#v#######v###.###v#
#...#.>.#...>.>.#.###.#
#####v#.#.###v#.#.###.#
#.....#...#...#.#.#...#
#.#########.###.#.#.###
#...###...#...#...#.###
###.###.#.###v#####v###
#...#...#.#.>.>.#.>.###
#.###.###.#.###.#.#v###
#.....###...###...#...#
#####################.#
)";
        Hiking test1{std::istringstream{TEST1}};
        expect(94_i == test1.Task1());
        expect(154_i == test1.Task2());

        Hiking task{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", task.Task1());
        if (Printer::EnableAll())
            Printer::Print(__FILE__, "2", task.Task2());
    };
};

} //namespace;
