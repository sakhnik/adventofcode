#include <iostream>
#include <unordered_map>
#include <cassert>
#include <algorithm>
#include <sstream>

int Calculate(std::istream &is)
{
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
            assert(!"Condition not implemented");

        if (op == "inc")
        {
            registers[reg1] += num;
        }
        else if (op == "dec")
        {
            registers[reg1] -= num;
        }
        else
            assert(!"Operation not implemented");
    }


    return std::max_element(registers.begin(), registers.end(),
                            [](const auto &a, const auto &b) { return a.second < b.second; })
        ->second;
}

int Calculate(std::istream &&is)
{
    return Calculate(is);
}

int main()
{
    assert(Calculate(std::istringstream(R"(b inc 5 if a > 1
a inc 1 if b < 5
c dec -10 if a >= 1
c inc -20 if c == 10)")) == 1);

    std::cout << Calculate(std::cin) << std::endl;
}
