#include "IntCode.h"
#include <sstream>
#include <numeric>
#include <algorithm>
#include <fstream>
#include "../test.hpp"
namespace {

using namespace boost::ut;

int Calculate(const IntCode &program, int phases[])
{
    std::vector<IntCode> amps(5, program);

    // Prime
    for (auto &a : amps)
    {
        a.Advance(0);
        expect(IntCode::S_INPUT == a.GetState());
    }

    // Configure the phases
    for (int i = 0; i < 5; ++i)
    {
        amps[i].Advance(phases[i]);
        expect(IntCode::S_INPUT == amps[i].GetState());
    }

    // Do a single amplification
    auto processInput = [&](auto &p, int i) {
        auto o = p.Advance(i);
        if (p.GetState() == IntCode::S_HALT)
        {
            throw "Halt";
        }
        expect(p.GetState() == IntCode::S_OUTPUT);
        p.Advance(0);
        bool isOk = IntCode::S_INPUT == p.GetState() || IntCode::S_HALT == p.GetState();
        expect(isOk);
        return o;
    };

    auto last{0};
    try
    {
        auto res{0};
        for (int i = 0; ; i = (i + 1) % 5)
        {
            if (!i)
            {
                last = res;
            }
            res = processInput(amps[i], res);
        }
    }
    catch (...)
    {
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

suite s = [] {
    "2019-07"_test = [] {
        {
            IntCode p{std::istringstream{"3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0"}};
            int phases[] = {4, 3, 2, 1, 0};
            expect(43210_i == Calculate(p, phases));
            expect(43210_i == FindMaxCombination(false, p));
        }

        {
            IntCode p{std::istringstream{R"(3,23,3,24,1002,24,10,24,1002,23,-1,23,
101,5,23,23,1,24,23,23,4,23,99,0,0)"}};
            expect(54321_i == FindMaxCombination(false, p));
        }

        {
            IntCode p{std::istringstream{R"(3,31,3,32,1002,32,10,32,1001,31,-2,31,1007,31,0,33,
1002,33,7,33,1,33,31,31,1,32,31,31,4,31,99,0,0,0)"}};
            expect(65210_i == FindMaxCombination(false, p));
        }

        IntCode program{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", FindMaxCombination(false, program));

        {
            IntCode p{std::istringstream{R"(3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,
27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5)"}};
            int phases[] = {9, 8, 7, 6, 5};
            expect(139629729_i == Calculate(p, phases));
            expect(139629729_i == FindMaxCombination(true, p));
        }

        {
            IntCode p{std::istringstream{R"(3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,
-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,
53,1001,56,-1,56,1005,56,6,99,0,0,0,0,10)"}};
            expect(18216_i == FindMaxCombination(true, p));
        }

        Printer::Print(__FILE__, "2", FindMaxCombination(true, program));
    };
};

} //namespace;