#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <vector>
#include <fstream>


struct Point
{
    int x,y;
};

struct Box
{
    int id;
    int x, y, w, h;
    int GetArea() const { return w * h; }
};

using BoxesT = std::vector<Box>;

BoxesT GetInput(std::istream &&is)
{
    BoxesT boxes;

    std::string line;
    while (is && (getline(is, line)))
    {
        Box box;
        REQUIRE(5 == sscanf(line.c_str(), "#%d @ %d,%d: %dx%d", &box.id, &box.x, &box.y, &box.w, &box.h));
        boxes.emplace_back(box);
    }

    return boxes;
}

BoxesT GetInput()
{
    return GetInput(std::ifstream{INPUT});
}

BoxesT GetInput(const char *s)
{
    return GetInput(std::istringstream{s});
}

std::pair<int,int> CalcIntersection(int length, const BoxesT &boxes)
{
    // To calculate the overlapping area, we'll apply every claim
    // to a discrete tissue. Then count the overlapping "pixels".
    std::vector<uint8_t> tissue(length * length, 0);

    auto applyBox = [&](const auto &box, auto &func) {
        for (int y = box.y, yn = box.y + box.h; y < yn; ++y)
        {
            int i = length * y + box.x;
            for (int j = 0; j < box.w; ++j)
            {
                func(i + j);
            }
        }
    };

    auto mark = [&](auto idx) {
        ++tissue[idx];
    };

    for (const auto &box : boxes)
    {
        applyBox(box, mark);
    }

    int overlap = std::count_if(begin(tissue), end(tissue), [](auto c) { return c > 1; });

    // To identify the unoverlapping claim, we'll need to look at the tissue again.
    // If the sum of counters equals to the area of the claim, no one else wanted it.
    for (const auto &box : boxes)
    {
        int sum{};
        auto accum = [&](auto idx) {
            sum += tissue[idx];
        };
        applyBox(box, accum);
        if (sum == box.GetArea())
        {
            return {overlap, box.id};
        }
    }


    return {overlap, -1};
}

TEST_CASE("main")
{
    auto test = CalcIntersection(8, GetInput("#1 @ 1,3: 4x4\n#2 @ 3,1: 4x4\n#3 @ 5,5: 2x2"));
    REQUIRE(4 == test.first);
    REQUIRE(3 == test.second);

    auto res = CalcIntersection(1024, GetInput());
    std::cout << res.first << std::endl;
    std::cout << res.second << std::endl;
}
