#include <doctest/doctest.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

struct Vec
{
    int x, y;
};

struct Light
{
    Vec position;
    Vec velocity;
};

struct Box
{
    Vec topleft;
    Vec bottomright;

    int Area() const
    {
        return (bottomright.x - topleft.x) * (bottomright.y - topleft.y);
    }

    int GetWidth() const
    {
        return bottomright.x - topleft.x + 1;
    }

    int GetHeight() const
    {
        return bottomright.y - topleft.y + 1;
    }
};

using SkyT = std::vector<Light>;

SkyT ParseInput(std::istream &&is)
{
    SkyT sky;
    std::string line;
    while (is && getline(is, line))
    {
        Light l;
        sscanf(line.c_str(), "position=<%d,%d> velocity=<%d,%d>",
               &l.position.x, &l.position.y, &l.velocity.x, &l.velocity.y);
        sky.push_back(l);
    }
    return sky;
}

Box GetBoundingBox(const SkyT &sky)
{
    auto MAX = std::numeric_limits<int>::max();
    auto MIN = std::numeric_limits<int>::min();
    Box box = {
        .topleft = { MAX, MAX },
        .bottomright = { MIN, MIN },
    };
    for (auto l : sky)
    {
        if (l.position.x < box.topleft.x) box.topleft.x = l.position.x;
        if (l.position.y < box.topleft.y) box.topleft.y = l.position.y;
        if (l.position.x > box.bottomright.x) box.bottomright.x = l.position.x;
        if (l.position.y > box.bottomright.y) box.bottomright.y = l.position.y;
    }
    return box;
}

void Evolve(SkyT &sky, int dt)
{
    for (auto &l : sky)
    {
        l.position.x += l.velocity.x * dt;
        l.position.y += l.velocity.y * dt;
    }
}

void Print(const SkyT &sky, std::ostream &os)
{
    auto box = GetBoundingBox(sky);
    auto width = box.GetWidth();
    auto height = box.GetHeight();
    std::vector<char> display(width * height, '.');

    auto idx = [width](auto x, auto y) {
        return y * width + x;
    };
    auto set = [&](int x, int y) {
        display.at(idx(x - box.topleft.x, y - box.topleft.y)) = '#';
    };

    for (auto l : sky)
    {
        set(l.position.x, l.position.y);
    }

    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            os << display[idx(w, h)];
        }
        os << "\n";
    }
}

int SearchMin(SkyT &sky)
{
    auto prev_area = GetBoundingBox(sky).Area();

    for (int i = 0; ; ++i)
    {
        Evolve(sky, 1);
        auto area = GetBoundingBox(sky).Area();
        if (area < 80*25 && area > prev_area)
        {
            Evolve(sky, -1);
            return i;
        }
        prev_area = area;
    }
}

TEST_CASE(TEST_NAME)
{
    const auto TEST = R"(position=< 9,  1> velocity=< 0,  2>
position=< 7,  0> velocity=<-1,  0>
position=< 3, -2> velocity=<-1,  1>
position=< 6, 10> velocity=<-2, -1>
position=< 2, -4> velocity=< 2,  2>
position=<-6, 10> velocity=< 2, -2>
position=< 1,  8> velocity=< 1, -1>
position=< 1,  7> velocity=< 1,  0>
position=<-3, 11> velocity=< 1, -2>
position=< 7,  6> velocity=<-1, -1>
position=<-2,  3> velocity=< 1,  0>
position=<-4,  3> velocity=< 2,  0>
position=<10, -3> velocity=<-1,  1>
position=< 5, 11> velocity=< 1, -2>
position=< 4,  7> velocity=< 0, -1>
position=< 8, -2> velocity=< 0,  1>
position=<15,  0> velocity=<-2,  0>
position=< 1,  6> velocity=< 1,  0>
position=< 8,  9> velocity=< 0, -1>
position=< 3,  3> velocity=<-1,  1>
position=< 0,  5> velocity=< 0, -1>
position=<-2,  2> velocity=< 2,  0>
position=< 5, -2> velocity=< 1,  2>
position=< 1,  4> velocity=< 2,  1>
position=<-2,  7> velocity=< 2, -2>
position=< 3,  6> velocity=<-1, -1>
position=< 5,  0> velocity=< 1,  0>
position=<-6,  0> velocity=< 2,  0>
position=< 5,  9> velocity=< 1, -2>
position=<14,  7> velocity=<-2,  0>
position=<-3,  6> velocity=< 2, -1>)";
    auto test = ParseInput(std::istringstream{TEST});

    std::ostringstream oss;
    REQUIRE(3 == SearchMin(test));
    Print(test, oss);
    REQUIRE(oss.str() == R"(#...#..###
#...#...#.
#...#...#.
#####...#.
#...#...#.
#...#...#.
#...#...#.
#...#..###
)");

    auto input = ParseInput(std::ifstream{INPUT});
    MESSAGE(SearchMin(input));
    Print(input, std::cout);
}
