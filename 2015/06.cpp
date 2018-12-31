#include <doctest/doctest.h>
#include <fstream>
#include <vector>
#include <regex>
#include <numeric>

namespace {

using LightsT = std::vector<int>;
const size_t WIDTH = 1000;

void On(int row, int col, LightsT &lights)
{
    lights[row * WIDTH + col] = 1;
}

void Off(int row, int col, LightsT &lights)
{
    lights[row * WIDTH + col] = 0;
}

void Toggle(int row, int col, LightsT &lights)
{
    auto &v = lights[row * WIDTH + col];
    v = 1 - v;
}

void On2(int row, int col, LightsT &lights)
{
    ++lights[row * WIDTH + col];
}

void Off2(int row, int col, LightsT &lights)
{
    auto &v = lights[row * WIDTH + col];
    if (v > 0)
        --v;
}

void Toggle2(int row, int col, LightsT &lights)
{
    auto &v = lights[row * WIDTH + col];
    v += 2;
}

template <typename OnT, typename OffT, typename ToggleT>
size_t CountLit(std::ifstream &&is, OnT on, OffT off, ToggleT toggle)
{
    LightsT lights(WIDTH*WIDTH, 0);

    std::string line;
    while (is && getline(is, line))
    {
        static const std::regex r(R"((turn on|turn off|toggle) (\d+),(\d+) through (\d+),(\d+))");
        std::smatch m;
        REQUIRE(std::regex_match(line, m, r));
        int r0 = std::stoi(m[2]);
        int c0 = std::stoi(m[3]);
        int r1 = std::stoi(m[4]);
        int c1 = std::stoi(m[5]);

        for (int r = r0; r <= r1; ++r)
        {
            for (int c = c0; c <= c1; ++c)
            {
                if (m[1] == "turn on")
                {
                    on(r, c, lights);
                }
                else if (m[1] == "turn off")
                {
                    off(r, c, lights);
                }
                else //if (m[1] == "toggle")
                {
                    toggle(r, c, lights);
                }
            }
        }
    }

    return std::accumulate(lights.begin(), lights.end(), 0u);
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("task") {
        MESSAGE(CountLit(std::ifstream{INPUT}, On, Off, Toggle));
        MESSAGE(CountLit(std::ifstream{INPUT}, On2, Off2, Toggle2));
    }
}
