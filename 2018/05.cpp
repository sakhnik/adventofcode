#include <fstream>
#include <algorithm>
#include <limits>
#include "../test.hpp"

namespace {

std::string Reduce(const std::string &s)
{
    std::string reduced;

    for (auto ch : s)
    {
        if (reduced.empty())
        {
            // The first unit is always sticked (maybe, temporarily).
            reduced.push_back(ch);
            continue;
        }

        // Check whether a pair of units can be reduced.
        if (std::abs(ch - reduced.back()) == 'a' - 'A')
        {
            reduced.pop_back();
            continue;
        }

        // If can't be reduced, stick with it.
        reduced.push_back(ch);
    }

    return reduced;
}

size_t Search(const std::string &s)
{
    size_t min_size = std::numeric_limits<int>::max();
    for (char c = 'A'; c <= 'Z'; ++c)
    {
        std::string test;
        std::copy_if(begin(s), end(s), std::back_inserter(test),
                     [&](char a) { return std::toupper(a) != c; });
        auto reduced = Reduce(test);
        if (size(reduced) < min_size)
        {
            min_size = size(reduced);
        }
    }
    return min_size;
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2018-05"_test = [] {
        expect(eq(""s, Reduce("aA")));
        expect(eq(""s, Reduce("abBA")));
        expect(eq("abAB"s, Reduce("abAB")));
        expect(eq("aabAAB"s, Reduce("aabAAB")));
        expect(eq("dabCBAcaDA"s, Reduce("dabAcCaCBAcCcaDA")));

        std::string input;
        getline(std::ifstream(INPUT), input);

        Printer::Print(__FILE__, "1", Reduce(input).size());

        expect(4_u == Search("dabAcCaCBAcCcaDA"));
        Printer::Print(__FILE__, "2", Search(input));
    };
};

} //namespace;
