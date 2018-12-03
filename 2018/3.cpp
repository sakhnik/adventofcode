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
    int x, y, w, h;
};

using BoxesT = std::vector<Box>;

BoxesT GetInput(std::istream &&is)
{
    BoxesT boxes;

    std::string line;
    while (is && (getline(is, line)))
    {
        int id{};
        Box box;
        REQUIRE(5 == sscanf(line.c_str(), "#%d @ %d,%d: %dx%d", &id, &box.x, &box.y, &box.w, &box.h));
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

int CalcIntersection(int length, const BoxesT &boxes)
{
    std::vector<uint8_t> tissue(length * length, 0);
    for (const auto &box : boxes)
    {
        for (int y = box.y, yn = box.y + box.h; y < yn; ++y)
        {
            int i = length * y + box.x;
            for (int j = 0; j < box.w; ++j)
            {
                ++tissue[i + j];
            }
        }
    }

    return std::count_if(begin(tissue), end(tissue), [](auto c) { return c > 1; });
}

TEST_CASE("main")
{
    REQUIRE(4 == CalcIntersection(8, GetInput("#1 @ 1,3: 4x4\n#2 @ 3,1: 4x4\n#3 @ 5,5: 2x2")));
    std::cout << CalcIntersection(1024, GetInput()) << std::endl;
}
