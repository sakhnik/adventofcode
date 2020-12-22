#include "../test.hpp"
#include <list>
#include <numeric>

namespace {

using DeckT = std::list<size_t>;

size_t Play(DeckT a, DeckT b)
{
    while (!a.empty() && !b.empty())
    {
        auto take = [](DeckT &win, DeckT &lose) {
            win.push_back(win.front());
            win.pop_front();
            win.push_back(lose.front());
            lose.pop_front();
        };

        if (a.front() > b.front())
            take(a, b);
        else
            take(b, a);
    }

    auto calc = [](const DeckT &d) {
        size_t i = 1;
        return std::accumulate(d.rbegin(), d.rend(), size_t{},
                               [&](auto s, auto v) {
                                    return s + v * i++;
                               });
    };

    return a.empty() ? calc(b) : calc(a);
}

using namespace boost::ut;

suite s = [] {
    "2020-22"_test = [] {

        {
            expect(306_u == Play({9, 2, 6, 3, 1}, {5, 8, 4, 7, 10}));;
        }

        auto score = Play(
            {29, 30, 44, 35, 27, 2, 4, 38,
             45, 33, 50, 21, 17, 11, 25, 40,
             5, 43, 41, 24, 12, 19, 23, 8, 42},
             {32, 13, 22, 7, 31, 16, 37, 6,
             10, 20, 47, 46, 34, 39, 1, 26,
             49, 9, 48, 36, 14, 15, 3, 18, 28}
        );
        Printer::Print(__FILE__, "1", score);
    };
};

} //namespace;
