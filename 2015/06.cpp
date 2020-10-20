#include <fstream>
#include <vector>
#include <numeric>
#include <boost/ut.hpp>

namespace {

using LightT = int;
using LightsT = std::vector<LightT>;
const size_t WIDTH = 1000;

void On(LightT *l)
{
    *l = 1;
}

void Off(LightT *l)
{
    *l = 0;
}

void Toggle(LightT *l)
{
    *l = 1 - *l;
}

void On2(LightT *l)
{
    ++*l;
}

void Off2(LightT *l)
{
    if (*l > 0)
        --*l;
}

void Toggle2(LightT *l)
{
    *l += 2;
}

template <typename OpT>
size_t CountLit(std::ifstream &&is, OpT on, OpT off, OpT toggle)
{
    LightsT lights(WIDTH*WIDTH, 0);

    std::string line;
    while (is && getline(is, line))
    {
        OpT op{};

        int r0{}, c0{}, r1{}, c1{};
        if (4 == sscanf(line.c_str(), "turn on %d,%d through %d,%d", &r0, &c0, &r1, &c1))
        {
            op = on;
        }
        else if (4 == sscanf(line.c_str(), "turn off %d,%d through %d,%d", &r0, &c0, &r1, &c1))
        {
            op = off;
        }
        else if (4 == sscanf(line.c_str(), "toggle %d,%d through %d,%d", &r0, &c0, &r1, &c1))
        {
            op = toggle;
        }

        for (int r = r0; r <= r1; ++r)
        {
            for (LightT *l{&lights[r * WIDTH + c0]}, *l1{l + (c1 - c0)}; l <= l1; ++l)
            {
                op(l);
            }
        }
    }

    return std::accumulate(lights.begin(), lights.end(), 0u);
}

using namespace boost::ut;

suite s = [] {
    "2015-06"_test = [] {
        std::cout << "2015-06.1: " << CountLit(std::ifstream{INPUT}, On, Off, Toggle) << std::endl;
        std::cout << "2015-06.2: " << CountLit(std::ifstream{INPUT}, On2, Off2, Toggle2) << std::endl;
    };
};

} //namespace;