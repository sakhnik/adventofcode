#include <doctest/doctest.h>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <numeric>
#include <sstream>


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
            assert(inserted);
        }
    }

    int CalcSumOfDepths()
    {
        return std::accumulate(begin(_map), end(_map), 0,
                               [&](int sum, auto &node) {
                                   return sum + _GetDepth(node.first);
                               });
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

TEST_CASE(TEST_NAME)
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

    REQUIRE(42 == Map{iss}.CalcSumOfDepths());

    std::ifstream ifs(INPUT);
    Map map{ifs};
    MESSAGE(map.CalcSumOfDepths());

}
