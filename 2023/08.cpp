#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

class Map
{
public:
    Map(std::istream &&is)
    {
        is >> navigation;
        std::string from, l, r, skip;
        while (is >> from >> skip >> l >> r)
        {
            std::string_view left{l.c_str() + 1, 3};
            std::string_view right{r.c_str(), 3};
            map[from] = {std::string{left}, std::string{right}};
        }
    }

    int Task1() const
    {
        std::string pos = "AAA";
        for (int i = 0; ; ++i)
        {
            if (pos == "ZZZ")
                return i;
            auto dir = navigation[i % navigation.size()];
            auto ways = map.at(pos);
            pos = dir == 'L' ? ways.first : ways.second;
        }
        return -1;
    }

    struct Path
    {
        int head{};
        int period{};
        std::vector<int> z;
    };

    Path Trace(std::string pos) const
    {
        Path path;
        std::unordered_map<std::string, std::unordered_map<int, int>> visited;
        for (int i = 0, j = 0; ; ++i, ++j)
        {
            if (j == navigation.size())
                j = 0;
            if (pos.ends_with('Z'))
                path.z.push_back(i);
            auto it = visited.find(pos);
            if (it != visited.end())
            {
                auto it2 = it->second.find(j);
                if (it2 != it->second.end())
                {
                    path.head = it2->second;
                    path.period = i - it2->second;
                    return path;
                }
            }
            visited[pos][j] = i;
            auto dir = navigation[j];
            auto ways = map.at(pos);
            pos = dir == 'L' ? ways.first : ways.second;
        }
        return {};
    }

    using BigIntT = std::size_t;

    BigIntT Task2() const
    {
        std::vector<Path> paths;
        for (const auto &node : map)
        {
            if (node.first.ends_with('A'))
            {
                paths.push_back(Trace(node.first));
                // simplification: turns out that the position of the first Z is equal to the period
                paths.back().period = paths.back().z[0];
            }
        }
        //for (const auto &p : paths)
        //{
        //    std::cerr << "head=" << p.head << " period=" << p.period << " finish=";
        //    for (auto z : p.z)
        //        std::cerr << " " << z;
        //    std::cerr << std::endl;
        //}
        BigIntT count{1};
        for (const auto &p : paths)
            count = std::lcm(count, p.period);
        return count;
    }

private:
    std::string navigation;
    std::unordered_map<std::string, std::pair<std::string, std::string>> map;
};

suite s = [] {
    "2023-08"_test = [] {
        const char *const TEST1 = R"(RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ)
)";
        const char *const TEST2 = R"(LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ)
)";
        const char *const TEST3 = R"(LR

11A = (11B, XXX)
11B = (XXX, 11Z)
11Z = (11B, XXX)
22A = (22B, XXX)
22B = (22C, 22C)
22C = (22Z, 22Z)
22Z = (22B, 22B)
XXX = (XXX, XXX)
)";
        Map test_map{std::istringstream{TEST1}};
        expect(2_i == test_map.Task1());
        Map test_map2{std::istringstream{TEST2}};
        expect(6_i == test_map2.Task1());
        Map test_map3{std::istringstream{TEST3}};
        expect(6_i == test_map3.Task2());

        Map map{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", map.Task1());
        Printer::Print(__FILE__, "2", map.Task2());
    };
};

} //namespace;
