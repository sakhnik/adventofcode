#include <doctest/doctest.h>
#include <sstream>
#include <fstream>
#include <array>
#include <vector>
#include <iostream>
#include <boost/pending/disjoint_sets.hpp>
#include <boost/iterator/counting_iterator.hpp>

namespace {

class Sky
{
public:
    Sky(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            _Pos pos{};
            REQUIRE(4 == sscanf(line.data(), "%d,%d,%d,%d", &pos[0], &pos[1], &pos[2], &pos[3]));
            _sky.push_back(pos);
        } 
    }

    int Count() const
    {
        std::vector<int> rank(_sky.size());
        std::vector<int> parent(_sky.size());
        boost::disjoint_sets<int*, int*> ds(&rank[0], &parent[0]);

        for (int i = 0, n = _sky.size(); i < n; ++i)
        {
            ds.make_set(i);
        }

        for (size_t i = 0, in = _sky.size() - 1; i < in; ++i)
        {
            for (size_t j = i + 1; j < _sky.size(); ++j)
            {
                if (_Dist(_sky[i], _sky[j]) <= 3)
                {
                    ds.link(i, j);
                }
            }
        }

        return ds.count_sets(boost::counting_iterator<int>(0),
                             boost::counting_iterator<int>(_sky.size()));
    }

private:
    using _Pos = std::array<int, 4>;
    using _Sky = std::vector<_Pos>;
    _Sky _sky;

    static int _Dist(const _Pos &a, const _Pos &b)
    {
        int sum{};
        for (int i = 0; i < 4; ++i)
        {
            sum += std::abs(a[i] - b[i]);
        }
        return sum;
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        Sky s(std::istringstream{
            " 0,0,0,0\n"
            " 3,0,0,0\n"
            " 0,3,0,0\n"
            " 0,0,3,0\n"
            " 0,0,0,3\n"
            " 0,0,0,6\n"
            " 9,0,0,0\n"
            "12,0,0,0\n"});
        REQUIRE(2 == s.Count());
    }

    SUBCASE("task") {
        Sky s(std::ifstream{INPUT});
        MESSAGE(s.Count());
    }
}
