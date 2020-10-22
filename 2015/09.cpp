#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>
#include "../test.hpp"
namespace {

class Tsp
{
public:
    Tsp(std::istream &&is)
    {
        std::string city, prev, skip;
        int dist;
        int i{0}, j{1};
        while (is && (is >> city >> skip >> skip >> skip >> dist))
        {
            if (prev.empty())
            {
                prev = city;
            }
            else if (prev != city)
            {
                prev = city;
                if (!_ncities)
                {
                    _ncities = j;
                }
                j = i + 2;
                ++i;
            }

            _dist[i][j] = dist;
            _dist[j][i] = dist;
            ++j;
        }

        for (int i = 0; i < _ncities; ++i)
        {
            _dist[i][i] = 0;
            //for (int j = 0; j < _ncities; ++j)
            //{
            //    std::cout << "  " << _dist[i][j];
            //}
            //std::cout << "\n";
        }
    }

    std::pair<int, int> Find() const
    {
        int idx[_ncities];
        std::iota(idx, idx + _ncities, 0);
        auto dist = [this](int *idx) {
            int d{};
            for (int i = 1; i < _ncities; ++i)
            {
                d += _dist[idx[i-1]][idx[i]];
            }
            return d;
        };
        int min_dist = dist(idx);
        int max_dist = min_dist;
        while (std::next_permutation(idx, idx + _ncities))
        {
            auto d2 = dist(idx);
            if (d2 < min_dist)
            {
                min_dist = d2;
            }
            if (d2 > max_dist)
            {
                max_dist = d2;
            }
        }
        return {min_dist, max_dist};
    }

private:
    int _ncities{};
    int _dist[10][10];
};

using namespace boost::ut;

suite s = [] {
    "2015-09.test"_test = [] {
        const char *const TEST =
            "London to Dublin = 464\n"
            "London to Belfast = 518\n"
            "Dublin to Belfast = 141\n";
        Tsp tsp(std::istringstream{TEST});
        auto d = tsp.Find();
        expect(605_i == d.first);
        expect(982_i == d.second);
    };

    "2015-09.task"_test = [] {
        Tsp tsp(std::ifstream{INPUT});
        auto d = tsp.Find();
        Printer::Print(__FILE__, "1", d.first);
        Printer::Print(__FILE__, "2", d.second);
    };
};

} //namespace;