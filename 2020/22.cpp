#include "../test.hpp"
#include <list>
#include <numeric>
#include <unordered_set>

namespace {

using DeckT = std::string;

size_t CalcScore(const DeckT &d)
{
    size_t i = 1;
    return std::accumulate(d.rbegin(), d.rend(), size_t{},
                           [&](auto s, auto v) {
                           return s + v * i++;
                           });
}

void TakeCards(DeckT &win, DeckT &lose)
{
    std::rotate(win.begin(), win.begin() + 1, win.end());
    win.push_back(lose.front());
    lose.erase(lose.begin());
}

size_t Play(DeckT a, DeckT b)
{
    while (!a.empty() && !b.empty())
    {
        if (a.front() > b.front())
            TakeCards(a, b);
        else
            TakeCards(b, a);
    }

    return a.empty() ? CalcScore(b) : CalcScore(a);
}

// true -- player1 wins
std::pair<bool, DeckT> PlayRecursive(DeckT a, DeckT b)
{
    auto decks2str = [&] {
        return a + '\xff' + b;
    };
    std::unordered_set<std::string> history{decks2str()};

    while (!a.empty() && !b.empty())
    {
        bool a_wins{true};

        if (a.size() > static_cast<size_t>(a.front())
            && b.size() > static_cast<size_t>(b.front()))
        {
            a_wins = PlayRecursive({a.begin() + 1, a.begin() + 1 + a.front()},
                                   {b.begin() + 1, b.begin() + 1 + b.front()}).first;
        }
        else
        {
            a_wins = a.front() > b.front();
        }

        if (a_wins)
            TakeCards(a, b);
        else
            TakeCards(b, a);

        if (history.contains(decks2str()))
            return {true, a};
        history.insert(decks2str());
    }

    return {b.empty(), b.empty() ? a : b};
}

size_t Play2(DeckT a, DeckT b)
{
    auto deck = PlayRecursive(a, b).second;
    return CalcScore(deck);
}

using namespace boost::ut;

suite s = [] {
    "2020-22"_test = [] {

        {
            expect(306_u == Play({9, 2, 6, 3, 1}, {5, 8, 4, 7, 10}));
            expect(291_u == Play2({9, 2, 6, 3, 1}, {5, 8, 4, 7, 10}));
        }

        DeckT a{29, 30, 44, 35, 27, 2, 4, 38,
             45, 33, 50, 21, 17, 11, 25, 40,
             5, 43, 41, 24, 12, 19, 23, 8, 42};
        DeckT b{32, 13, 22, 7, 31, 16, 37, 6,
             10, 20, 47, 46, 34, 39, 1, 26,
             49, 9, 48, 36, 14, 15, 3, 18, 28};
        Printer::Print(__FILE__, "1", Play(a, b));
        Printer::Print(__FILE__, "2", Play2(a, b));
    };
};

} //namespace;
