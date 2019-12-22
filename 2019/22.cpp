#include <doctest/doctest.h>
#include <fstream>
#include <numeric>
#include <vector>
#include <string_view>
#include <algorithm>
#include <sstream>
#include <iostream>


using DeckT = std::vector<int>;

DeckT Deal(int count, std::istream &is)
{
    DeckT deck(count), deck2(count);
    std::iota(begin(deck), end(deck), 0);

    std::string line;
    while (getline(is, line))
    {
        if (line == "deal into new stack")
        {
            std::reverse(begin(deck), end(deck));
        }
        else if (std::string_view{line.data(), 4} == "cut ")
        {
            auto num = std::stoi(line.substr(4));
            if (num < 0)
                num += count;
            CHECK(num >= 0);
            CHECK(num < count);
            std::rotate(begin(deck), begin(deck) + num, end(deck));
        }
        else
        {
            const std::string INCR = "deal with increment ";
            CHECK(std::string_view{line.data(), INCR.size()} == INCR);
            auto num = std::stoi(line.substr(INCR.size()));
            for (int i{0}, idx{0}; i < count; ++i, idx += num)
            {
                if (idx > count)
                    idx -= count;
                deck2[idx] = deck[i];
            }
            deck.swap(deck2);
        }
    }

    return deck;
}

TEST_CASE(TEST_NAME)
{
    {
        std::istringstream iss{
            "deal with increment 7\n"
            "deal into new stack\n"
            "deal into new stack\n"
        };
        DeckT ref{0, 3, 6, 9, 2, 5, 8, 1, 4, 7};
        REQUIRE(ref == Deal(10, iss));
    }

    {
        std::istringstream iss{"cut 3\n"};
        DeckT ref{3, 4, 5, 6, 7, 8, 9, 0, 1, 2};
        REQUIRE(ref == Deal(10, iss));
    }

    {
        std::istringstream iss{
            "cut 6\n"
            "deal with increment 7\n"
            "deal into new stack\n"
        };
        DeckT ref{3, 0, 7, 4, 1, 8, 5, 2, 9, 6};
        REQUIRE(ref == Deal(10, iss));
    }

    {
        std::istringstream iss{
            "deal with increment 7\n"
            "deal with increment 9\n"
            "cut -2\n"
        };
        DeckT ref{6, 3, 0, 7, 4, 1, 8, 5, 2, 9};
        REQUIRE(ref == Deal(10, iss));
    }

    {
        std::istringstream iss{
            "deal into new stack\n"
            "cut -2\n"
            "deal with increment 7\n"
            "cut 8\n"
            "cut -4\n"
            "deal with increment 7\n"
            "cut 3\n"
            "deal with increment 9\n"
            "deal with increment 3\n"
            "cut -1\n"
        };
        DeckT ref{9, 2, 5, 8, 1, 4, 7, 0, 3, 6};
        REQUIRE(ref == Deal(10, iss));
    }

    {
        std::ifstream ifs{INPUT};
        auto deck = Deal(10007, ifs);
        auto it = std::find(begin(deck), end(deck), 2019);
        MESSAGE(it - begin(deck));
    }
}
