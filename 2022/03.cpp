#include "../test.hpp"
#include <fstream>

namespace {

const char *const TEST = R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw
)";

int GetIndex(char item)
{
    return item <= 'Z' ? ((item - 'A') + 26) : item - 'a';
}

int FindDuplicates(std::istream &&is)
{
    int total_priority{};

    std::string line;
    while (std::getline(is, line))
    {
        std::array<int, 26 * 2> items;
        items.fill(0);
        for (int i = 0, n = line.size() / 2; i != n; ++i)
        {
            int p = GetIndex(line[i]);
            items[p] = 1;
        }
        for (int i = line.size() / 2; i != line.size(); ++i)
        {
            int p = GetIndex(line[i]);
            if (items[p])
            {
                total_priority += p + 1;
                break;
            }
        }
    }

    return total_priority;
}

int FindIntersection(std::istream &&is)
{
    int total_priority{};
    do
    {
        std::array<int, 26 * 2> items;
        items.fill(0);
        for (int i = 0; i < 3; ++i)
        {
            std::array<int, 26 * 2> dedup;
            dedup.fill(0);

            std::string line;
            is >> line;
            for (char item : line)
            {
                int p = GetIndex(item);
                if (!dedup[p])
                {
                    dedup[p] = 1;
                    ++items[p];
                }
            }
        }
        auto it = std::find(items.begin(), items.end(), 3);
        if (it != items.end())
        {
            int p = it - items.begin();
            total_priority += 1 + p;
        }
    }
    while (is);
    return total_priority;
}

using namespace boost::ut;

suite s = [] {
    "2022-03"_test = [] {
        expect(157_i == FindDuplicates(std::istringstream{TEST}));
        expect(70_i == FindIntersection(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", FindDuplicates(std::ifstream{INPUT}));
        Printer::Print(__FILE__, "2", FindIntersection(std::ifstream{INPUT}));
    };
};

} //namespace;
