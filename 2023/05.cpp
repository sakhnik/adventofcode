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
            Mapping mapping{};
            std::istringstream iss{line};
            iss >> mapping.delta >> mapping.src >> mapping.len;
            mapping.delta -= mapping.src;
            maps.back().mappings.push_back(mapping);
        }

        for (auto &m : maps)
            m.Sort();
    }

    int64_t Task1() const
    {
        int64_t min = std::numeric_limits<int64_t>::max();
        for (auto &seed : seeds)
            min = std::min(min, Map(seed));
        return min;
    }

    int64_t Task2() const
    {
        int64_t min = std::numeric_limits<int64_t>::max();
        for (int i = 0; i < seeds.size(); i += 2)
            min = std::min(min, Map(Range{seeds[i], seeds[i+1]}));
        return min;
    }

private:
    std::vector<int64_t> seeds;

    struct Range
    {
        int64_t src{}, len{};
        bool operator<(const Range &o) const { return src < o.src; }
        bool operator==(const Range &o) const { return src == o.src; }
    };

    struct Mapping : Range
    {
        int64_t delta{};
    };

    struct Mappings
    {
        std::vector<Mapping> mappings;

        void Sort()
        {
            std::sort(mappings.begin(), mappings.end());
        }

        int64_t Map(int64_t val) const
        {
            for (const auto &mapping : mappings)
            {
                auto nv = val - mapping.src;
                if (nv >= 0 && nv < mapping.len)
                    return val + mapping.delta;
            }
            return val;
        }

        std::vector<Range> Map(Range range) const
        {
            std::vector<Range> res;

            auto process = [&](Mapping m, auto &process_ref) {
                if (m.src + m.len <= range.src)
                    return true;
                if (m.src >= range.src + range.len)
                {
                    res.push_back(range);
                    return false;
                }
                if (m.src < range.src)
                {
                    auto d = range.src - m.src;
                    return process_ref({{range.src, m.len - d}, m.delta}, process_ref);
                }
                if (m.src == range.src)
                {
                    auto cut = std::min(m.len, range.len);
                    res.push_back({range.src + m.delta, cut});
                    range.src += cut;
                    range.len -= cut;
                    return range.len != 0;
                }
                auto cut = m.src - range.src;
                res.push_back({range.src, cut});
                range.src += cut;
                range.len -= cut;
                return process_ref(m, process_ref);
            };

            for (const auto &mapping : mappings)
            {
                if (!process(mapping, process))
                    break;
            }
            if (range.len)
                res.push_back(range);

            std::sort(res.begin(), res.end());
            return res;
        }

        std::vector<Range> Map(const std::vector<Range> &ranges) const
        {
            std::vector<Range> res;
            for (auto range : ranges)
                for (auto r : Map(range))
                    res.push_back(r);
            std::sort(res.begin(), res.end());
            return res;
        }
    };

    std::vector<Mappings> maps;

    int64_t Map(int64_t val) const
    {
        for (auto &mappings : maps)
            val = mappings.Map(val);
        return val;
    }

    int64_t Map(Range range) const
    {
        std::vector ranges{range};
        for (auto &mappings : maps)
            ranges = mappings.Map(ranges);
        return ranges[0].src;
    }
};

suite s = [] {
    "05"_test = [] {
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
        expect(46_i == test_almanac.Task2());

        Almanac almanac{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", almanac.Task1());
        Printer::Print(__FILE__, "2", almanac.Task2());
    };
};

} //namespace;
