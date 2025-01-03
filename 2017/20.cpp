#include <fstream>
#include <algorithm>
#include <vector>
#include "../test.hpp"

namespace {

struct Vec
{
    int x, y, z;

    bool operator==(const Vec &o) const
    {
        return x == o.x && y == o.y && z == o.z;
    }

    Vec& operator+=(const Vec &o)
    {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }
};

struct Part
{
    Vec p, v, a;
    unsigned number;
};

typedef std::vector<Part> SwarmT;

SwarmT Parse(std::istream &is)
{
    SwarmT swarm;
    std::string line;
    unsigned number{0};
    while (getline(is, line))
    {
        Part part;
        part.number = number++;
        sscanf(line.c_str(),
               "p=<%d,%d,%d>, v=<%d,%d,%d>, a=<%d,%d,%d>",
               &part.p.x, &part.p.y, &part.p.z,
               &part.v.x, &part.v.y, &part.v.z,
               &part.a.x, &part.a.y, &part.a.z);
        swarm.push_back(part);
    }
    return swarm;
}

unsigned Distance(const Vec &a)
{
    return std::abs(a.x) + std::abs(a.y) + std::abs(a.z);
}

SwarmT FilterLowestAcceleration(SwarmT swarm)
{
    std::sort(swarm.begin(), swarm.end(),
              [](const auto &p1, const auto &p2) { return Distance(p1.a) < Distance(p2.a); });
    auto it = std::remove_if(swarm.begin(), swarm.end(),
                             [&](const auto &p) { return Distance(p.a) > Distance(swarm[0].a); });
    swarm.erase(it, swarm.end());
    return swarm;
}

unsigned FindClosest(SwarmT swarm)
{
    for (unsigned i = 0; i < 10000; ++i)
    {
        for (auto &p : swarm)
        {
            p.v += p.a;
            p.p += p.v;
        }

    }

    auto it = std::min_element(swarm.begin(), swarm.end(),
                               [](const auto &p1, const auto &p2) { return Distance(p1.p) < Distance(p2.p); });
    return it->number;
}

bool StrictOrder(const Vec &v1, const Vec &v2)
{
    if (v1.x < v2.x)
        return true;
    if (v1.x == v2.x)
    {
        if (v1.y < v2.y)
            return true;
        if (v1.y == v2.y)
        {
            return v1.z < v2.z;
        }
    }
    return false;
}

bool StrictOrderByPosition(const Part &p1, const Part &p2)
{
    return StrictOrder(p1.p, p2.p);
}

unsigned SimulateCollisions(SwarmT swarm)
{
    SwarmT aux;
    for (unsigned i = 0; i < 10000; ++i)
    {
        for (auto &p : swarm)
        {
            p.v += p.a;
            p.p += p.v;
        }

        aux.clear();
        std::sort(swarm.begin(), swarm.end(), StrictOrderByPosition);
        for (unsigned i = 0; i < swarm.size(); ++i)
        {
            unsigned j = i + 1;
            while (j < swarm.size() && swarm[i].p == swarm[j].p)
                ++j;
            if (j == i + 1)
                aux.push_back(swarm[i]);
            i = j - 1;
        }
        swarm.swap(aux);
    }

    return swarm.size();
}

using namespace boost::ut;

suite s = [] {
    "20"_test = [] {
        std::ifstream ifs(INPUT);
        auto swarm = Parse(ifs);
        Printer::Print(__FILE__, "1", FindClosest(FilterLowestAcceleration(swarm)));
        Printer::Print(__FILE__, "2", SimulateCollisions(swarm));
    };
};

} //namespace;
