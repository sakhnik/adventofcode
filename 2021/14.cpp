#include "../test.hpp"

namespace {

class Polymer
{
public:
    Polymer(std::istream &&is)
    {
        std::string line;
        getline(is, line);
        std::adjacent_find(line.begin(), line.end(),
                [this](char a, char b) { ++_pair_counts[{a, b}]; return false; });
        for (char c : line)
            ++_link_counts[c];

        while (getline(is, line))
        {
            char buf[64], l{};
            if (2 == sscanf(line.c_str(), "%s -> %c", buf, &l))
                _links[buf] = l;
        }
    }

    Polymer& Iterate()
    {
        decltype(_pair_counts) next_pair_counts;
        for (const auto &[p, c] : _pair_counts)
        {
            auto it = _links.find(p);
            if (it != _links.end())
            {
                next_pair_counts[{p[0], it->second}] += c;
                next_pair_counts[{it->second, p[1]}] += c;
                _link_counts[it->second] += c;
            }
        }

        std::swap(_pair_counts, next_pair_counts);
        return *this;
    }

    Polymer& Iterate(int count)
    {
        while (count--)
            Iterate();
        return *this;
    }

    size_t CountScore() const
    {
        auto [it_min, it_max] = std::minmax_element(_link_counts.begin(), _link_counts.end(),
                [](auto &a, auto &b) { return a.second < b.second; });
        return it_max->second - it_min->second;
    }

private:
    std::unordered_map<std::string, size_t> _pair_counts;
    std::unordered_map<char, size_t> _link_counts;
    std::unordered_map<std::string, char> _links;
};

const char *TEST_INPUT = R"(NNCB

CH -> B
HH -> N
CB -> H
NH -> C
HB -> C
HC -> B
HN -> C
NN -> C
BH -> H
NC -> B
NB -> B
BN -> B
BB -> N
BC -> B
CC -> N
CN -> C
)";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-14"_test = [] {
        Polymer test{std::istringstream{TEST_INPUT}};
        test.Iterate(10);
        expect(1588_u == test.CountScore());
        test.Iterate(30);
        expect(eq(2188189693529ull, test.CountScore()));

        Polymer polymer{std::ifstream{INPUT}};
        polymer.Iterate(10);
        Printer::Print(__FILE__, "1", polymer.CountScore());
        polymer.Iterate(30);
        Printer::Print(__FILE__, "2", polymer.CountScore());
    };
};

} //namespace;
