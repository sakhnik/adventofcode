#include <doctest/doctest.h>
#include <array>
#include <sstream>
#include <fstream>
#include <bitset>
#include <iostream>
#include <algorithm>

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

struct Result
{
    int task1;
    int task2;
};

Result Count(std::istream &&is)
{
    using OpSetT = std::bitset<16>;
    std::array<OpSetT, 16> op_mask;
    for (size_t i = 0; i < size(op_mask); ++i)
    {
        op_mask[i].set();
    }

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

        OpSetT op_set;
        int matches{0};
        for (int o = 0; o < 16; ++o)
        {
            if (r2 == Eval2(o, a, b, c, r1))
            {
                ++matches;
                op_set.set(o);
            }
        }

        op_mask[op] &= op_set;

        if (matches >= 3)
            ++count;

        std::getline(is, l1);
    }

    std::array<int, 16> op_map{0};
    while (true)
    {
        auto it = std::find_if(begin(op_mask), end(op_mask), [](auto m) { return m.count() == 1; });
        if (it == end(op_mask))
            break;
        int opcode = it - begin(op_mask);
        auto mask = it->to_ulong();
        int code = 0;
        while (mask != 1)
        {
            mask >>= 1;
            ++code;
        }
        op_map[opcode] = code;
        for (size_t i = 0; i < size(op_mask); ++i)
            op_mask[i].reset(code);
    }

    RegsT regs{};
    int op{}, a{}, b{}, c{};
    while (is && (is >> op >> a >> b >> c))
    {
        Eval(op_map[op], a, b, c, regs);
    }

    return {count, regs[0]};
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    auto result = Count(std::ifstream{INPUT});
    MESSAGE(result.task1);
    MESSAGE(result.task2);
}
