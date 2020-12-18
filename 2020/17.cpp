#include "../test.hpp"
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace {

using Point3 = std::array<int, 3>;
using Point4 = std::array<int, 4>;

template <typename P>
struct PointHash
{
    size_t operator()(const P &p) const noexcept
    {
        size_t seed{};
        for (auto x : p)
            boost::hash_combine(seed, x);
        return seed;
    }
};

template <typename PointT>
class Life
{
public:
    Life(std::istream &&is)
    {
        int y{};

        std::string line;
        while (getline(is, line))
        {
            for (int x = 0; x < line.size(); ++x)
            {
                if (line[x] == '#')
                    _points.insert({x, y, 0});
            }
            ++y;
        }
    }

    void Evolve()
    {
        std::unordered_map<PointT, int, PointHash<PointT>> counts;
        for (const auto &p : _points)
        {
            std::function<void(size_t i, PointT, PointT)> calc_counts = [&](size_t i, PointT dp, PointT np) {
                if (i == dp.size())
                {
                    if (dp == PointT{})
                        return;
                    ++counts[np];
                    return;
                }

                auto x = np[i];
                for (int j = -1; j <= 1; ++j)
                {
                    dp[i] = j;
                    np[i] = x + j;
                    calc_counts(i + 1, dp, np);
                }
            };
            calc_counts(0, PointT{}, p);
        }

        _PointsT new_points;
        for (const auto &pc : counts)
        {
            auto it = _points.find(pc.first);
            if (it == _points.end())
            {
                if (pc.second == 3)
                    new_points.insert(pc.first);
            }
            else
            {
                if (pc.second == 2 || pc.second == 3)
                    new_points.insert(pc.first);
            }
        }

        _points.swap(new_points);
    }

    void Evolve(size_t rounds)
    {
        while (rounds-- > 0)
            Evolve();
    }

    size_t CountActive() const { return _points.size(); }

private:
    using _PointsT = std::unordered_set<PointT, PointHash<PointT>>;
    _PointsT _points;
};

using namespace boost::ut;

suite s = [] {
    "2020-17"_test = [] {
        const char *const TEST = ".#.\n..#\n###";
        Life<Point3> l{std::istringstream{TEST}};
        l.Evolve(6);
        expect(112_u == l.CountActive());

        Life<Point4> l4{std::istringstream{TEST}};
        l4.Evolve(6);
        expect(848_u == l4.CountActive());

        Life<Point3> life{std::ifstream{INPUT}};
        life.Evolve(6);
        Printer::Print(__FILE__, "1", life.CountActive());

        Life<Point4> life4{std::ifstream{INPUT}};
        life4.Evolve(6);
        Printer::Print(__FILE__, "2", life4.CountActive());
    };
};

} //namespace;
