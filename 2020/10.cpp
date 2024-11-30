#include "../test.hpp"
#include <vector>
#include <fstream>
#include <unordered_map>
#include <numeric>

namespace {

class Adapters
{
public:
    Adapters(std::istream &&is)
    {
        int rating{};
        while (is && (is >> rating))
        {
            _ratings.push_back(rating);
        }
        std::sort(_ratings.begin(), _ratings.end());
    }

    template <typename IterT>
    Adapters(IterT first, IterT last)
        : _ratings(first, last)
    {
        std::sort(_ratings.begin(), _ratings.end());
    }

    int CalcDistribution() const
    {
        int diff1{}, diff3{};
        std::accumulate(_ratings.begin(), _ratings.end(), 0, [&](int prev, int cur) {
                            if (cur - prev == 1)
                                ++diff1;
                            else if (cur - prev == 3)
                                ++diff3;
                            return cur;
                        });
        ++diff3;
        return diff1 * diff3;
    }

    uint64_t CalcCombinations() const
    {
        std::unordered_map<int, uint64_t> counts;
        counts[0] = 1;

        // 1,       4, 5, 6, 7,           10
        //
        // Counts:
        // 1  0  0  0  0  0  0  0  0
        // 1  0  0  1  0  0  0  0  0
        // 1  0  0  1  1  0  0  0  0
        // 1  0  0  1  1  1  0  0  0
        // 1  0  0  1  1  1  3  0  0
        for (int r : _ratings)
        {
            counts[r] = counts[r - 1] + counts[r - 2] + counts[r - 3];
        }

        return counts[_ratings.back()];;
    }

private:
    std::vector<int> _ratings;
};

using namespace boost::ut;

suite s = [] {
    "10"_test = [] {
        {
            const int r[] = {16, 10, 15, 5, 1, 11, 7, 19, 6, 12, 4};
            Adapters a{std::begin(r), std::end(r)};
            expect(35_i == a.CalcDistribution());
            expect(8_u == a.CalcCombinations());
        }

        {
            const int r[] = {
                28, 33, 18, 42, 31, 14, 46, 20, 48, 47,
                24, 23, 49, 45, 19, 38, 39, 11, 1, 32,
                25, 35, 8, 17, 7, 9, 4, 2, 34, 10, 3
            };
            Adapters a{std::begin(r), std::end(r)};
            expect(220_i == a.CalcDistribution());
            expect(19208_u == a.CalcCombinations());
        }

        Adapters a{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", a.CalcDistribution());
        Printer::Print(__FILE__, "2", a.CalcCombinations());
    };
};

} //namespace;
