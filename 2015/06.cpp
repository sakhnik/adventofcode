#include <doctest/doctest.h>
#include <fstream>
#include <vector>
#include <regex>
#include <numeric>

namespace {

class Lights
{
public:
    Lights(std::ifstream &&is)
        : _lights(1000*1000, 0)
    {
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
                        _On(r, c);
                    }
                    else if (m[1] == "turn off")
                    {
                        _Off(r, c);
                    }
                    else //if (m[1] == "toggle")
                    {
                        _Toggle(r, c);
                    }
                }
            }
        }
    }

    size_t GetLitCount() const
    {
        return std::accumulate(_lights.begin(), _lights.end(), 0u);
    }

private:
    std::vector<uint8_t> _lights;

    void _On(int row, int col)
    {
        _lights[row * 1000 + col] = 1;
    }

    void _Off(int row, int col)
    {
        _lights[row * 1000 + col] = 0;
    }

    void _Toggle(int row, int col)
    {
        auto &v = _lights[row * 1000 + col];
        v = 1 - v;
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("task") {
        MESSAGE(Lights(std::ifstream{INPUT}).GetLitCount());
    }
}
