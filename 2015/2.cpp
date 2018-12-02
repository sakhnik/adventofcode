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

int CalcArea(const BoxesT &boxes)
{
    return std::accumulate(begin(boxes), end(boxes), 0,
                           [](int a, const auto &b) { return a + CalcArea(b); });
}

TEST_CASE("main")
{
    REQUIRE(58 == CalcArea({2,3,4}));
    REQUIRE(43 == CalcArea({1,1,10}));

    std::cout << CalcArea(GetInput()) << std::endl;
}
