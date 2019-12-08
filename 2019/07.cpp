#include <doctest/doctest.h>
#include "IntCode.h"
#include <sstream>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iostream>


int Calculate(const IntCode &program, int phases[])
{
    std::vector<IntCode> amps(5, program);

    // Prime
    for (auto &a : amps)
    {
        CHECK(IntCode::S_INPUT == a.Advance(0));
    }

    // Configure the phases
    for (int i = 0; i < 5; ++i)
    {
        CHECK(IntCode::S_INPUT == amps[i].Advance(phases[i]));
    }

    // Do a single amplification
    auto processInput = [&](auto &p, int i) {
        auto o = p.Advance(i);
        if (o == IntCode::S_HALT)
        {
            return o;
        }
        auto r = p.Advance(0);
        bool isOk = IntCode::S_INPUT == r || IntCode::S_HALT == r;
        CHECK(isOk);
        return o;
    };

    auto last{0};
    auto res{0};
    for (int i = 0; res != IntCode::S_HALT; i = (i + 1) % 5)
    {
        if (!i)
        {
            last = res;
        }
        res = processInput(amps[i], res);
    }

    return last;
}

int FindMaxCombination(bool feedback, const IntCode &program)
{
    int phases[] = {0, 1, 2, 3, 4};
    if (feedback)
    {
        for (int i = 0; i < 5; ++i)
        {
            phases[i] += 5;
        }
    }

    int result{};
    do
    {
        auto val = Calculate(program, phases);
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
        REQUIRE(43210 == Calculate(p, phases));
        REQUIRE(43210 == FindMaxCombination(false, p));
    }

    {
        IntCode p{std::istringstream{R"(3,23,3,24,1002,24,10,24,1002,23,-1,23,
101,5,23,23,1,24,23,23,4,23,99,0,0)"}};
        REQUIRE(54321 == FindMaxCombination(false, p));
    }

    {
        IntCode p{std::istringstream{R"(3,31,3,32,1002,32,10,32,1001,31,-2,31,1007,31,0,33,
1002,33,7,33,1,33,31,31,1,32,31,31,4,31,99,0,0,0)"}};
        REQUIRE(65210 == FindMaxCombination(false, p));
    }

    IntCode program{std::ifstream{INPUT}};
    MESSAGE(FindMaxCombination(false, program));

    {
        IntCode p{std::istringstream{R"(3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,
27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5)"}};
        int phases[] = {9,8,7,6,5};
        REQUIRE(139629729 == Calculate(p, phases));
        REQUIRE(139629729 == FindMaxCombination(true, p));
    }

    {
        IntCode p{std::istringstream{R"(3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,
-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,
53,1001,56,-1,56,1005,56,6,99,0,0,0,0,10)"}};
        REQUIRE(18216 == FindMaxCombination(true, p));
    }

    MESSAGE(FindMaxCombination(true, program));
}
