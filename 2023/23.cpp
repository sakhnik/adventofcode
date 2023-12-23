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

    int Task1()
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
            return map[pos[0]][pos[1]];
        };

        auto follow = [&](const PosT &pos, int dir) -> PosT {
            return { pos[0] + dp[dir][0], pos[1] + dp[dir][1]};
        };

        auto getDirections = [&](const PosT &pos, int dir) -> std::string {
            int opposite_dir = (dir + 2) % 4;
            char cur_place = getPlace(pos);
            auto forced_dir = std::string{">v<^"}.find_first_of(cur_place);
            if (forced_dir != std::string::npos)
                return opposite_dir == forced_dir ? "" : std::string{char(forced_dir)};
            std::string res;
            for (int i : {0, 1, 2, 3})
                if (i != opposite_dir && getPlace(follow(pos, i)) != '#')
                    res.push_back(i);
            return res;
        };

        auto trace = [&](PosT pos, int dir, int dist, auto &&trace) {
            if (pos == target)
            {
                longest_walk = std::max(longest_walk, dist);
                return;
            }
            auto directions = getDirections(pos, dir);
            if (directions.empty())
                return;
            if (directions.size() == 1)
            {
                trace(follow(pos, directions[0]), directions[0], dist + 1, trace);
                return;
            }

            if (visited.count(pos))
                return;
            visited.insert(pos);

            for (auto next_dir : directions)
                trace(follow(pos, next_dir), next_dir, dist + 1, trace);

            visited.erase(pos);
        };
        trace(start, 1, 0, trace);

        return longest_walk;
    }

    int Task2()
    {
        return 0;
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
        //expect(7_i == test1.Task2());

        Hiking task{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", task.Task1());
        //Printer::Print(__FILE__, "2", task.Task2());
    };
};

} //namespace;
