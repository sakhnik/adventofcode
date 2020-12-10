#include "../test.hpp"
#include <set>
#include <fstream>

namespace {

class Adapters
{
public:
    Adapters(std::istream &&is)
    {
        int rating{};
        while (is && (is >> rating))
        {
            _ratings.emplace(rating);
        }
    }

    template <typename IterT>
    Adapters(IterT first, IterT last)
        : _ratings(first, last)
    {
    }

    int CalcDistribution() const
    {
        int diff1{}, diff3{};
        std::accumulate(_ratings.begin(), _ratings.end(), 0, [&](int cur, int r) {
                            if (r - cur == 1)
                                ++diff1;
                            else if (r - cur == 3)
                                ++diff3;
                            return r;
                        });
        ++diff3;
        return diff1 * diff3;
    }

private:
    std::set<int> _ratings;
};

using namespace boost::ut;

suite s = [] {
    "2020-10"_test = [] {
        {
            const int r[] = {16, 10, 15, 5, 1, 11, 7, 19, 6, 12, 4};
            Adapters a{std::begin(r), std::end(r)};
            expect(35_i == a.CalcDistribution());
        }

        {
            const int r[] = {
                28, 33, 18, 42, 31, 14, 46, 20, 48, 47,
                24, 23, 49, 45, 19, 38, 39, 11, 1, 32,
                25, 35, 8, 17, 7, 9, 4, 2, 34, 10, 3
            };
            Adapters a{std::begin(r), std::end(r)};
            expect(220_i == a.CalcDistribution());
        }

        Adapters a{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", a.CalcDistribution());
    };
};

} //namespace;
