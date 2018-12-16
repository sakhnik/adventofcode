#include <doctest/doctest.h>
#include <array>
#include <sstream>
#include <fstream>

namespace {

using RegsT = std::array<int, 4>;

void Eval(int op, int a, int b, int c, RegsT &regs)
{
    switch (op)
    {
    case 0:  regs[c] = regs[a] + regs[b];  break;  // addr
    case 1:  regs[c] = regs[a] + b;        break;  // addi
    case 2:  regs[c] = regs[a] * regs[b];  break;  // mulr
    case 3:  regs[c] = regs[a] * b;        break;  // muli
    case 4:  regs[c] = regs[a] & regs[b];  break;  // banr
    case 5:  regs[c] = regs[a] & b;        break;  // bani
    case 6:  regs[c] = regs[a] | regs[b];  break;  // borr
    case 7:  regs[c] = regs[a] | b;        break;  // bori
    case 8:  regs[c] = regs[a];            break;  // setr
    case 9:  regs[c] = a;                  break;  // seti
    case 10: regs[c] = a > regs[b];        break;  // gtir
    case 11: regs[c] = regs[a] > b;        break;  // gtri
    case 12: regs[c] = regs[a] > regs[b];  break;  // gtrr
    case 13: regs[c] = a == regs[b];       break;  // eqir
    case 14: regs[c] = regs[a] == b;       break;  // eqri
    case 15: regs[c] = regs[a] == regs[b]; break;  // eqrr
    }
}

RegsT Eval2(int op, int a, int b, int c, RegsT regs)
{
    Eval(op, a, b, c, regs);
    return regs;
}

int Count(std::istream &&is)
{
    int count{};

    std::string l1, l2, l3;
    while (is
           && std::getline(is, l1)
           && std::getline(is, l2)
           && std::getline(is, l3))
    {
        RegsT r1;
        if (4 != sscanf(l1.data(), "Before: [%d, %d, %d, %d]", &r1[0], &r1[1], &r1[2], &r1[3]))
            break;
        int op{}, a{}, b{}, c{};
        if (4 != sscanf(l2.data(), "%d %d %d %d", &op, &a, &b, &c))
            break;
        RegsT r2;
        if (4 != sscanf(l3.data(), "After: [%d, %d, %d, %d]", &r2[0], &r2[1], &r2[2], &r2[3]))
            break;

        int matches{0};
        for (int o = 0; o < 16; ++o)
        {
            if (r2 == Eval2(o, a, b, c, r1))
                ++matches;
        }

        if (matches >= 3)
            ++count;

        std::getline(is, l1);
    }

    return count;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("task1") {
        MESSAGE(Count(std::ifstream{INPUT}));
    }
}
