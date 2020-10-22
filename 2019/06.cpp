#include <fstream>
#include <cassert>
#include <unordered_map>
#include <numeric>
#include <sstream>
#include "../test.hpp"
namespace {

class Map
{
public:
    Map(std::istream &is)
    {
        _map.emplace("COM", Data{"", 0});
        std::string line;
        while (getline(is, line))
        {
            auto sep = line.find(')');
            assert(sep != line.npos);
            auto parent = line.substr(0, sep);
            auto obj = line.substr(sep + 1);
            auto [it, inserted] = _map.emplace(obj, Data{parent});
            boost::ut::expect(inserted);
        }
    }

    int CalcSumOfDepths()
    {
        return std::accumulate(begin(_map), end(_map), 0,
                               [&](int sum, auto &node) {
                                   return sum + _GetDepth(node.first);
                               });
    }

    int CalcDistance(const std::string &a, const std::string &b) const
    {
        const auto &dataA = _map.find(a)->second;
        const auto &dataB = _map.find(b)->second;
        if (dataA.parent == dataB.parent)
        {
            return 0;
        }
        if (dataA.depth > dataB.depth)
        {
            return 1 + CalcDistance(dataA.parent, b);
        }
        if (dataA.depth < dataB.depth)
        {
            return 1 + CalcDistance(a, dataB.parent);
        }
        return 2 + CalcDistance(dataA.parent, dataB.parent);
    }

private:
    struct Data
    {
        std::string parent;
        int depth{-1};
    };
    std::unordered_map<std::string, Data> _map;

    int _GetDepth(const std::string &obj)
    {
        auto it = _map.find(obj);
        assert(it != _map.end());

        auto &data = it->second;
        if (data.depth != -1)
        {
            return data.depth;
        }
        return data.depth = 1 + _GetDepth(data.parent);
    }
};

using namespace boost::ut;

suite s = [] {
    "2019-06"_test = [] {
        {
            std::istringstream iss(R"(COM)B
B)C
C)D
D)E
E)F
B)G
G)H
D)I
E)J
J)K
K)L)");

            expect(42_i == Map{iss}.CalcSumOfDepths());
        }

        std::ifstream ifs(INPUT);
        Map map{ifs};
        Printer::Print(__FILE__, "1", map.CalcSumOfDepths());

        {
            std::istringstream iss(R"(COM)B
B)C
C)D
D)E
E)F
B)G
G)H
D)I
E)J
J)K
K)L
K)YOU
I)SAN)");
            Map map(iss);
            map.CalcSumOfDepths();
            expect(4_i == map.CalcDistance("YOU", "SAN"));
        }

        Printer::Print(__FILE__, "2", map.CalcDistance("YOU", "SAN"));
    };
};

} //namespace;