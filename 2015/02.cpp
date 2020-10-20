#include <fstream>
#include <numeric>
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/ut.hpp>


namespace {

using namespace boost::ut;

using BoxT = std::array<int, 3>;
using BoxesT = std::vector<BoxT>;

BoxesT GetInput()
{
    BoxesT boxes;

    std::ifstream ifs(INPUT);
    BoxT box;
    char x;
    while (ifs && (ifs >> box[0] >> x >> box[1] >> x >> box[2]))
    {
        std::sort(begin(box), end(box));
        boxes.push_back(box);
    }

    return boxes;
}

int CalcArea(const BoxT &box)
{
    expect(std::is_sorted(begin(box), end(box)));
    return 3*box[0]*box[1] + 2*(box[1]*box[2] + box[0]*box[2]);
}

int CalcRibbon(const BoxT &box)
{
    expect(std::is_sorted(begin(box), end(box)));
    return 2*(box[0] + box[1]) + box[0]*box[1]*box[2];
}

template <typename Func>
int Accumulate(const BoxesT &boxes, Func func)
{
    return std::accumulate(begin(boxes), end(boxes), 0,
                           [func](int a, const auto &b) { return a + func(b); });
}

suite s = [] {
    "2015-02"_test = [] {
        expect(58_i == CalcArea({2, 3, 4}));
        expect(43_i == CalcArea({1, 1, 10}));

        std::cout << "2015-02.1: " << Accumulate(GetInput(), CalcArea) << std::endl;

        expect(34_i == CalcRibbon({2, 3, 4}));
        expect(14_i == CalcRibbon({1, 1, 10}));

        std::cout << "2015-02.2: " << Accumulate(GetInput(), CalcRibbon) << std::endl;
    };
};

} //namespace;