#include "../test.hpp"
#include <fstream>
#include <boost/functional/hash.hpp>

namespace {

struct Droplet
{
    struct Cube
    {
        std::array<int, 3> x;
        bool operator==(const Cube &o) const = default;

        Cube GetNext(int i, int j) const
        {
            Cube c{*this};
            c.x[i] += j;
            return c;
        }

        bool OutOf(const Cube &b1, const Cube &b2) const
        {
            for (int i = 0; i < 3; ++i)
            {
                if (x[i] < b1.x[i] || x[i] > b2.x[i])
                    return true;
            }
            return false;
        }
    };

    struct CubeHash
    {
        size_t operator()(const Cube &c) const
        {
            size_t seed{};
            for (auto x : c.x)
                boost::hash_combine(seed, x);
            return seed;
        }
    };

    std::unordered_set<Cube, CubeHash> cubes;

    Droplet(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            Cube cube{};
            sscanf(line.c_str(), "%d,%d,%d", &cube.x[0], &cube.x[1], &cube.x[2]);
            cubes.insert(cube);
        }
    }

    int CalcArea() const
    {
        int area{};
        for (const auto &cube : cubes)
        {
            for (int i = 0; i < 3; ++i)
            {
                for (int j : {-1, 1})
                {
                    Cube neighbour = cube.GetNext(i, j);
                    if (!cubes.contains(neighbour))
                        ++area;
                }
            }
        }
        return area;
    }

    int CalcExternalArea() const
    {
        Cube b1{}, b2{};
        for (const auto &cube : cubes)
        {
            for (int i = 0; i < 3; ++i)
            {
                b1.x[i] = std::min(b1.x[i], cube.x[i] - 1);
                b2.x[i] = std::max(b2.x[i], cube.x[i] + 1);
            }
        }

        std::unordered_set<Cube, CubeHash> exterior;
        exterior.insert(b1);
        std::queue<Cube> bfs;
        bfs.push(b1);
        while (!bfs.empty())
        {
            Cube cube = bfs.front();
            bfs.pop();

            for (int i = 0; i < 3; ++i)
            {
                for (auto j : {-1, 1})
                {
                    auto next = cube.GetNext(i, j);
                    if (next.OutOf(b1, b2))
                        continue;
                    if (exterior.contains(next) || cubes.contains(next))
                        continue;
                    exterior.insert(next);
                    bfs.push(next);
                }
            }
        }

        int area{};
        for (const auto &cube : cubes)
        {
            for (int i = 0; i < 3; ++i)
            {
                for (int j : {-1, 1})
                {
                    Cube next = cube.GetNext(i, j);
                    if (exterior.contains(next))
                        ++area;
                }
            }
        }
        return area;
    }
};

const char *const TEST = R"(2,2,2
1,2,2
3,2,2
2,1,2
2,3,2
2,2,1
2,2,3
2,2,4
2,2,6
1,2,5
3,2,5
2,1,5
2,3,5)";

using namespace boost::ut;

suite s = [] {
    "2022-18"_test = [] {
        Droplet test{std::istringstream{TEST}};
        expect(64_i == test.CalcArea());
        expect(58_i == test.CalcExternalArea());

        Droplet droplet{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", droplet.CalcArea());
        Printer::Print(__FILE__, "2", droplet.CalcExternalArea());
    };
};

} //namespace;
