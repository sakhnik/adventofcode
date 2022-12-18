#include "../test.hpp"
#include <fstream>
#include <boost/functional/hash.hpp>

namespace {

struct Droplet
{
    struct Cube
    {
        int x{}, y{}, z{};

        bool operator==(const Cube &o) const = default;
    };

    struct CubeHash
    {
        size_t operator()(const Cube &c) const
        {
            size_t seed{};
            boost::hash_combine(seed, c.x);
            boost::hash_combine(seed, c.y);
            boost::hash_combine(seed, c.z);
            return seed;
        }
    };

    std::unordered_set<Cube, CubeHash> cubes;

    Droplet(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            int x{}, y{}, z{};
            sscanf(line.c_str(), "%d,%d,%d", &x, &y, &z);
            cubes.insert({x, y, z});
        }
    }

    int GetArea() const
    {
        int area{};
        for (const auto &cube : cubes)
        {
            for (int i : {-1, 1})
            {
                if (!cubes.contains({cube.x + i, cube.y, cube.z}))
                    ++area;
                if (!cubes.contains({cube.x, cube.y + i, cube.z}))
                    ++area;
                if (!cubes.contains({cube.x, cube.y, cube.z + i}))
                    ++area;
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
        expect(64_i == test.GetArea());

        Droplet droplet{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", droplet.GetArea());
    };
};

} //namespace;
