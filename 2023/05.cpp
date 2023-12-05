#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

class Almanac
{
public:
    Almanac(std::istream &&is)
    {
        std::string line;
        getline(is, line);
        std::istringstream iss{line};
        std::string discard;
        iss >> discard;
        int64_t num{};
        while (iss >> num)
            seeds.push_back(num);

        while (getline(is, line))
        {
            if (line.empty())
                continue;

            if (line.ends_with("map:"))
            {
                maps.emplace_back();
                continue;
            }
            Range range{};
            std::istringstream iss{line};
            iss >> range.dst >> range.src >> range.len;
            maps.back().ranges.push_back(range);
        }
    }

    int64_t Task1() const
    {
        int64_t min = std::numeric_limits<int64_t>::max();
        for (auto &seed : seeds)
            min = std::min(min, Map(seed));
        return min;
    }

private:
    std::vector<int64_t> seeds;

    struct Range
    {
        int64_t src{}, dst{}, len{};
    };

    struct Ranges
    {
        std::vector<Range> ranges;

        int64_t Map(int64_t val) const
        {
            for (const auto &range : ranges)
            {
                auto nv = val - range.src;
                if (nv >= 0 && nv < range.len)
                    return nv + range.dst;
            }
            return val;
        }
    };

    std::vector<Ranges> maps;

    int64_t Map(int64_t val) const
    {
        for (auto &ranges : maps)
            val = ranges.Map(val);
        return val;
    }
};

suite s = [] {
    "2023-05"_test = [] {
        const char *const TEST1 = R"(seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4
)";
        Almanac test_almanac{std::istringstream{TEST1}};
        expect(35_i == test_almanac.Task1());
        //expect(30_u == test_game.GetTotalCopies());

        Almanac almanac{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", almanac.Task1());
        //Printer::Print(__FILE__, "2", game.GetTotalCopies());
    };
};

} //namespace;
