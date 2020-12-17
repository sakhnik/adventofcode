#include "../test.hpp"
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace {

struct Point
{
    int x, y, z;

    bool operator==(const Point &o) const
    {
        return x == o.x && y == o.y && z == o.z;
    }
};

struct PointHash
{
    size_t operator()(const Point &p) const noexcept
    {
        size_t seed{};
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        boost::hash_combine(seed, p.z);
        return seed;
    }
};

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
        std::unordered_map<Point, int, PointHash> counts;
        for (const auto &p : _points)
        {
            for (int dx = -1; dx <= 1; ++dx)
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dz = -1; dz <= 1; ++dz)
                        if (dx || dy || dz)
                            ++counts[{p.x + dx, p.y + dy, p.z + dz}];
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
    using _PointsT = std::unordered_set<Point, PointHash>;
    _PointsT _points;
};

using namespace boost::ut;

suite s = [] {
    "2020-17"_test = [] {
        const char *const TEST = ".#.\n..#\n###";
        Life l{std::istringstream{TEST}};
        l.Evolve(6);
        expect(112_u == l.CountActive());
    };

    Life life{std::ifstream{INPUT}};
    life.Evolve(6);
    Printer::Print(__FILE__, "1", life.CountActive());
};

} //namespace;
