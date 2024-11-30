#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "../test.hpp"
namespace {

using namespace boost::ut;

struct Result
{
    int max = 0;
    int max_ever = 0;
};

Result Calculate(std::istream &is)
{
    Result result;
    std::unordered_map<std::string, int> registers;

    //b inc 5 if a > 1
    std::string reg1, op, skip, reg2, cond;
    int num{0}, arg{0};

    while (is >> reg1 >> op >> num >> skip >> reg2 >> cond >> arg)
    {
        auto v = registers[reg2];
        if (cond == "==")
        {
            if (v != arg)
                continue;
        }
        else if (cond == "!=")
        {
            if (v == arg)
                continue;
        }
        else if (cond == "<")
        {
            if (v >= arg)
                continue;
        }
        else if (cond == ">")
        {
            if (v <= arg)
                continue;
        }
        else if (cond == "<=")
        {
            if (v > arg)
                continue;
        }
        else if (cond == ">=")
        {
            if (v < arg)
                continue;
        }
        else
            expect(!"Condition not implemented");

        if (op == "inc")
        {
            registers[reg1] += num;
        }
        else if (op == "dec")
        {
            registers[reg1] -= num;
        }
        else
            expect(!"Operation not implemented");

        if (result.max_ever < registers[reg1])
            result.max_ever = registers[reg1];
    }

    result.max = std::max_element(registers.begin(), registers.end(),
                                  [](const auto &a, const auto &b) { return a.second < b.second; })
        ->second;
    return result;
}

Result Calculate(std::istream &&is)
{
    return Calculate(is);
}

suite s = [] {
    "08"_test = [] {
        auto test = Calculate(std::istringstream(R"(b inc 5 if a > 1
a inc 1 if b < 5
c dec -10 if a >= 1
c inc -20 if c == 10)"));
        expect(1_i == test.max);
        expect(10_i == test.max_ever);

        std::ifstream ifs(INPUT);
        auto res = Calculate(ifs);
        Printer::Print(__FILE__, "1", res.max);
        Printer::Print(__FILE__, "2", res.max_ever);
    };
};

} //namespace;
