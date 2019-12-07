#include <doctest/doctest.h>
#include "IntCode.h"
#include <sstream>
#include <numeric>
#include <algorithm>
#include <fstream>


template <typename IterT>
int Calculate(const IntCode &program, IterT begin, IterT end)
{
    int res{};

    for (; begin != end; ++begin)
    {
        std::istringstream iss(std::to_string(*begin) + " " + std::to_string(res));
        std::ostringstream oss;
        IntCode amp{program};
        amp.Run(iss, oss);
        res = std::stoi(oss.str());
    }

    return res;
}

int FindMaxCombination(const IntCode &program)
{
    int phases[] = {0, 1, 2, 3, 4};
    int result{};
    do
    {
        auto val = Calculate(program, std::begin(phases), std::end(phases));
        if (val > result)
        {
            result = val;
        }
    }
    while (std::next_permutation(std::begin(phases), std::end(phases)));
    return result;
}

TEST_CASE(TEST_NAME)
{
    {
        IntCode p{std::istringstream{"3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0"}};
        int phases[] = {4,3,2,1,0};
        REQUIRE(43210 == Calculate(p, phases, phases + 5));
        REQUIRE(43210 == FindMaxCombination(p));
    }

    {
        IntCode p{std::istringstream{R"(3,23,3,24,1002,24,10,24,1002,23,-1,23,
101,5,23,23,1,24,23,23,4,23,99,0,0)"}};
        REQUIRE(54321 == FindMaxCombination(p));
    }

    {
        IntCode p{std::istringstream{R"(3,31,3,32,1002,32,10,32,1001,31,-2,31,1007,31,0,33,
1002,33,7,33,1,33,31,31,1,32,31,31,4,31,99,0,0,0)"}};
        REQUIRE(65210 == FindMaxCombination(p));
    }

    IntCode program{std::ifstream{INPUT}};
    MESSAGE(FindMaxCombination(program));
}
