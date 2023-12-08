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
        Map test_map{std::istringstream{TEST1}};
        expect(2_i == test_map.Task1());
        Map test_map2{std::istringstream{TEST2}};
        expect(6_i == test_map2.Task1());
        //expect(5905_i == test_cards.Task2());

        Map map{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", map.Task1());
        //Printer::Print(__FILE__, "2", cards.Task2());
    };
};

} //namespace;
