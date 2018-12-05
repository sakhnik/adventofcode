#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <list>
#include <fstream>

using PolymerT = std::list<char>;

std::string GetInput(std::istream &&is)
{
    std::string line;
    getline(is, line);
    return line;
}

std::string Reduce(const std::string &s)
{
    PolymerT poly(begin(s), end(s));

    auto it = begin(poly);
    while (it != end(poly))
    {
        // Consider a pair of units.
        auto it2 = it++;
        if (it == end(poly))
        {
            break;
        }

        // Check whether they can be reduced.
        if (std::abs(*it - *it2) == 'a' - 'A')
        {
            poly.erase(it);
            it = poly.erase(it2);
            // Make sure we check whether the preceding one could be reduced too.
            if (it != begin(poly))
            {
                --it;
            }
        }

    }

    return std::string(begin(poly), end(poly));
}

TEST_CASE("main")
{
    REQUIRE("" == Reduce("aA"));
    REQUIRE("" == Reduce("abBA"));
    REQUIRE("abAB" == Reduce("abAB"));
    REQUIRE("aabAAB" == Reduce("aabAAB"));
    REQUIRE("dabCBAcaDA" == Reduce("dabAcCaCBAcCcaDA"));

    std::string input;
    getline(std::ifstream(INPUT), input);

    std::cout << Reduce(input).size() << std::endl;
}
