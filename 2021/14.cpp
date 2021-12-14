#include "../test.hpp"

namespace {

class Polymer
{
public:
    Polymer(std::istream &&is)
    {
        std::string line;
        getline(is, line);
        _chain.assign(line.begin(), line.end());

        while (getline(is, line))
        {
            char buf[64], l{};
            if (2 == sscanf(line.c_str(), "%s -> %c", buf, &l))
                _links[buf] = l;
        }
    }

    Polymer& Iterate()
    {
        for (auto it = _chain.begin(), it_prev = it++; it != _chain.end(); ++it, ++it_prev)
        {
            std::string p{*it_prev};
            p.push_back(*it);
            auto j = _links.find(p);
            if (j != _links.end())
                it_prev = _chain.insert(it, j->second);
        }
        return *this;
    }

    Polymer& Iterate(int count)
    {
        while (count--)
            Iterate();
        return *this;
    }

    std::string Print()
    {
        return {_chain.begin(), _chain.end()};
    }

    size_t CountScore() const
    {
        std::unordered_map<char, size_t> counts;
        for (auto l : _chain)
            ++counts[l];
        auto [it_min, it_max] = std::minmax_element(counts.begin(), counts.end(),
                [](auto &a, auto &b) { return a.second < b.second; });
        return it_max->second - it_min->second;
    }

private:
    std::list<char> _chain;
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
        expect("NCNBCHB"s == test.Iterate().Print());
        expect(1_u == test.CountScore());
        test.Iterate(9);
        expect(1588_u == test.CountScore());

        Polymer polymer{std::ifstream{INPUT}};
        polymer.Iterate(10);
        Printer::Print(__FILE__, "1", polymer.CountScore());
    };
};

} //namespace;
