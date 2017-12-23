#include "Asm.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <fstream>

unsigned CountMuls(const ProgramT &program)
{
    uint64_t count{0};
    std::vector<int64_t> regs('h' - 'a' + 1, 0);

    auto get_value = [&](Arg a) {
        if (a.is_reg)
            return regs[a.a.reg - 'a'];
        return a.a.num;
    };

    for (size_t i = 0; i < program.size(); ++i)
    {
        using Op = Instr::Op;
        auto instr = program[i];
        auto X = instr.X;
        auto Y = instr.Y;

        switch (instr.op)
        {
        case Op::Set:
            //set X Y sets register X to the value of Y.
            regs[X.a.reg - 'a'] = get_value(Y);
            break;
        case Op::Sub:
            //sub X Y decreases register X by the value of Y.
            regs[X.a.reg - 'a'] -= get_value(Y);
            break;
        case Op::Mul:
            //mul X Y sets register X to the result of multiplying the value contained in register X by the value of Y.
            regs[X.a.reg - 'a'] *= get_value(Y);
            ++count;
            break;
        case Op::Jnz:
            if (get_value(X) != 0)
                i += get_value(Y) - 1;
            break;
        default:
            break;
        }
    }

    return count;
}

TEST_CASE("main")
{
    std::ifstream ifs(INPUT);
    auto program = Parse(ifs);

    std::cout << CountMuls(program) << std::endl;

}
