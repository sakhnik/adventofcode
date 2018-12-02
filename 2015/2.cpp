#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <fstream>
#include <numeric>

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
    REQUIRE(std::is_sorted(begin(box), end(box)));
    return 3*box[0]*box[1] + 2*(box[1]*box[2] + box[0]*box[2]);
}

int CalcRibbon(const BoxT &box)
{
    REQUIRE(std::is_sorted(begin(box), end(box)));
    return 2*(box[0] + box[1]) + box[0]*box[1]*box[2];
}

template <typename Func>
int Accumulate(const BoxesT &boxes, Func func)
{
    return std::accumulate(begin(boxes), end(boxes), 0,
                           [func](int a, const auto &b) { return a + func(b); });
}

TEST_CASE("main")
{
    REQUIRE(58 == CalcArea({2,3,4}));
    REQUIRE(43 == CalcArea({1,1,10}));

    std::cout << Accumulate(GetInput(), CalcArea) << std::endl;

    REQUIRE(34 == CalcRibbon({2,3,4}));
    REQUIRE(14 == CalcRibbon({1,1,10}));

    std::cout << Accumulate(GetInput(), CalcRibbon) << std::endl;
}
