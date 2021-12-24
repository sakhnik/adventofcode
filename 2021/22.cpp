#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

using PointT = std::array<int, 3>;

struct Cube
{
    PointT p1;
    PointT p2;

    Cube(PointT p1, PointT p2)
        : p1{p1}
        , p2{p2}
    {
    }

    bool operator==(const Cube &o) const
    {
        return p1 == o.p1 && p2 == o.p2;
    }

    bool operator<(const Cube &o) const
    {
        return p1 < o.p1 || (p1 == o.p1 && p2 < o.p2);
    }

    bool Intersects(const Cube &o) const
    {
        for (int i = 0; i < 3; ++i)
        {
            // If distance between centers is more than the sum of half widths,
            // no intersection along this axis.
            auto c1 = p2[i] + p1[i];
            auto c2 = o.p2[i] + o.p1[i];
            auto w1 = p2[i] - p1[i];
            auto w2 = o.p2[i] - o.p1[i];
            if (std::abs(c2 - c1) >= w1 + w2)
                return false;
        }
        return true;
    }

    bool Crosses(const Cube &o) const
    {
        if (!Intersects(o))
            return false;
        for (int i = 0; i < 3; ++i)
        {
            if ((p1[i] < o.p1[i] && o.p1[i] < p2[i]) || (p1[i] < o.p2[i] && o.p2[i] < p2[i]))
                return true;
        }
        return false;
    }

    std::list<Cube> Slice(const Cube &o) const
    {
        std::list<Cube> cubes{*this};
        for (int i = 0; i < 3; ++i)
        {
            for (int p : {o.p1[i], o.p2[i]})
            {
                for (auto it = cubes.begin(); it != cubes.end(); )
                {
                    if (it->p1[i] < p && it->p2[i] > p)
                    {
                        auto to_split = *it;
                        it = cubes.erase(it);
                        PointT pm1 = to_split.p2;
                        pm1[i] = p;
                        it = cubes.insert(it, Cube{to_split.p1, pm1});
                        PointT pm2 = to_split.p1;
                        pm2[i] = p;
                        it = cubes.insert(++it, Cube{pm2, to_split.p2});
                    }
                    ++it;
                }
            }
        }
        return cubes;
    }

    size_t Volume() const
    {
        size_t v{1};
        for (int i = 0; i < 3; ++i)
            v *= p2[i] - p1[i];
        return v;
    }
};

std::ostream& operator<<(std::ostream &os, const PointT &p)
{
    return os << p[0] << "," << p[1] << "," << p[2];
}

std::ostream& operator<<(std::ostream &os, const Cube &c)
{
    return os << c.p1 << ":" << c.p2;
}

struct Geometry
{
    using CubesT = std::list<Cube>;
    CubesT cubes;

    Geometry(CubesT &&cubes)
        : cubes{cubes}
    {
    }

    size_t size() const { return cubes.size(); }

    size_t Volume() const
    {
        size_t v{};
        for (const auto &c : cubes)
            v += c.Volume();
        return v;
    }

    void Reduce()
    {
        cubes.sort();
        for (auto it2 = cubes.begin(), it1{it2++}; it2 != cubes.end(); ++it2)
        {
            if (*it1 == *it2)
                it1 = cubes.erase(it1);
            else
                ++it1;
        }
    }

    void Slice(const Cube &o)
    {
        for (auto it = cubes.begin(); it != cubes.end(); )
        {
            if (!it->Crosses(o))
                ++it;
            else
            {
                auto sl = it->Slice(o);
                it = cubes.erase(it);
                cubes.splice(it, sl);
            }
        }
        //for (auto &c : cubes)
        //    assert(!c.Crosses(o));
    }

    void Add(const Cube &o)
    {
        Slice(o);
        Geometry g({o});
        for (const auto &c : cubes)
            g.Slice(c);
        cubes.splice(cubes.end(), g.cubes);
        Reduce();
    }

    void Remove(const Cube &o)
    {
        Slice(o);
        for (auto it = cubes.begin(); it != cubes.end(); )
        {
            if (it->Intersects(o))
                it = cubes.erase(it);
            else
                ++it;
        }
    }

    void Intersect(const Cube &o)
    {
        Slice(o);
        for (auto it = cubes.begin(); it != cubes.end(); )
        {
            if (!it->Intersects(o))
                it = cubes.erase(it);
            else
                ++it;
        }
    }
};

class Reactor
{
public:
    using CmdT = std::pair<std::string, Cube>;
    using CmdsT = std::vector<CmdT>;
    CmdsT cmds;

    Reactor(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            char cmd[64];
            Cube c{{}, {}};
            if (7 == sscanf(line.c_str(), "%s x=%d..%d,y=%d..%d,z=%d..%d",
                    cmd,
                    &c.p1[0], &c.p2[0], &c.p1[1], &c.p2[1], &c.p1[2], &c.p2[2]))
            {
                for (auto &p : c.p2)
                    ++p;
                cmds.emplace_back(cmd, c);
            }
        }
    }

    size_t Execute(const Cube &core, size_t max_count = std::numeric_limits<size_t>::max()) const
    {
        Geometry reactor{{}};
        for (const auto &[cmd, cube] : cmds)
        {
            if (cmd == "on")
            {
                reactor.Add(cube);
                reactor.Intersect(core);
            }
            else if (cmd == "off")
                reactor.Remove(cube);
            if (!--max_count)
                break;
        }
        return reactor.Volume();
    }

    size_t ExecuteTest(const Cube &core, size_t max_count = std::numeric_limits<size_t>::max()) const
    {
        std::unordered_set<PointT, boost::hash<PointT>> points;

        for (const auto &[cmd, cube] : cmds)
        {
            for (int x = std::max(cube.p1[0], core.p1[0]), x_end = std::min(cube.p2[0], core.p2[0]);
                    x < x_end; ++x)
            {
                for (int y = std::max(cube.p1[1], core.p1[1]), y_end = std::min(cube.p2[1], core.p2[1]);
                        y < y_end; ++y)
                {
                    for (int z = std::max(cube.p1[2], core.p1[2]), z_end = std::min(cube.p2[2], core.p2[2]);
                            z < z_end; ++z)
                    {
                        if (cmd == "on")
                            points.insert({x,y,z});
                        else
                            points.erase({x,y,z});
                    }
                }
            }

            if (!--max_count)
                break;
        }
        return points.size();
    }
};

const char *const TEST = R"(
on x=-20..26,y=-36..17,z=-47..7
on x=-20..33,y=-21..23,z=-26..28
on x=-22..28,y=-29..23,z=-38..16
on x=-46..7,y=-6..46,z=-50..-1
on x=-49..1,y=-3..46,z=-24..28
on x=2..47,y=-22..22,z=-23..27
on x=-27..23,y=-28..26,z=-21..29
on x=-39..5,y=-6..47,z=-3..44
on x=-30..21,y=-8..43,z=-13..34
on x=-22..26,y=-27..20,z=-29..19
off x=-48..-32,y=26..41,z=-47..-37
on x=-12..35,y=6..50,z=-50..-2
off x=-48..-32,y=-32..-16,z=-15..-5
on x=-18..26,y=-33..15,z=-7..46
off x=-40..-22,y=-38..-28,z=23..41
on x=-16..35,y=-41..10,z=-47..6
off x=-32..-23,y=11..30,z=-14..3
on x=-49..-5,y=-3..45,z=-29..18
off x=18..30,y=-20..-8,z=-3..13
on x=-41..9,y=-7..43,z=-33..15
on x=-54112..-39298,y=-85059..-49293,z=-27449..7877
on x=967..23432,y=45373..81175,z=27513..53682
)";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-22"_test = [] {
        {
            Cube c1{{0,0,0}, {2,2,2}};
            expect(c1.Intersects(c1));
            expect(8_u == c1.Volume());
            Cube c2{{3,3,3}, {4,4,4}};
            expect(!c1.Intersects(c2));

            auto sl1 = c1.Slice(c1);
            expect(1_u == sl1.size());
            expect(eq(c1, sl1.front()));

            Cube c3{{0,0,0}, {1,1,1}};
            expect(c1.Intersects(c3));
            Geometry sl2 = c1.Slice(c3);
            expect(8_u == sl2.size());
            expect(8_u == sl2.Volume());

            Cube c4{{1,1,1}, {3,3,3}};
            expect(c1.Intersects(c4));
            Geometry sl3 = c1.Slice(c4);
            expect(8_u == sl3.size());
            expect(8_u == sl3.Volume());
        }

        {
            Geometry g{{}};
            g.Add({{10,10,10}, {13,13,13}});
            expect(27_u == g.Volume());
            g.Add({{11,11,11}, {14,14,14}});
            expect(46_u == g.Volume());
            g.Remove({{9,9,9}, {12,12,12}});
            expect(38_u == g.Volume());
            g.Add({{10,10,10}, {11,11,11}});
            expect(39_u == g.Volume());
        }

        Cube test_core{{-50, -50, -50}, {51, 51, 51}};
        Reactor test{std::istringstream{TEST}};
        //size_t count = 6;
        //expect(eq(test.ExecuteTest(test_core, count), test.Execute(test_core, count)));
        expect(590784_u == test.ExecuteTest(test_core));

        Reactor reactor{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", reactor.ExecuteTest(test_core));
    };
};

} //namespace;
