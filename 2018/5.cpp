#include <doctest/doctest.h>
#include <fstream>
#include <algorithm>
#include <iostream>

std::string GetInput(std::istream &&is)
{
    std::string line;
    getline(is, line);
    return line;
}

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

int Search(const std::string &s)
{
    int min_size = std::numeric_limits<int>::max();
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

TEST_CASE(TEST_NAME)
{
    REQUIRE("" == Reduce("aA"));
    REQUIRE("" == Reduce("abBA"));
    REQUIRE("abAB" == Reduce("abAB"));
    REQUIRE("aabAAB" == Reduce("aabAAB"));
    REQUIRE("dabCBAcaDA" == Reduce("dabAcCaCBAcCcaDA"));

    std::string input;
    getline(std::ifstream(INPUT), input);

    std::cout << Reduce(input).size() << std::endl;

    REQUIRE(4 == Search("dabAcCaCBAcCcaDA"));
    std::cout << Search(input) << std::endl;
}
