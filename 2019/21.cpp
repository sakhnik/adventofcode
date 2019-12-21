#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>


int Survey(const char *script, IntCode prog)
{
    int input = 0;
    while (prog.GetState() != prog.S_HALT)
    {
        auto r = prog.Advance(input);
        switch (prog.GetState())
        {
        case IntCode::S_RUN:
        case IntCode::S_HALT:
            continue;
        case IntCode::S_OUTPUT:
            if (r >= 128)
                return r;
            std::cout << static_cast<char>(r) << std::flush;
            break;
        case IntCode::S_INPUT:
            input = *script++;
            break;
        }
    }
    return -1;
}

TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCode prog{ifs};

    // The bot always jumps 4 squares ahead.
    // So it should jump as soon as a hole is detected,
    // but having the ground on the fourth tile.
    const char *walk =
        "NOT T T\n"  // T = true
        "AND A T\n"  // T = A
        "AND B T\n"  // T = A && B
        "AND C T\n"  // T = A && B && C
        "NOT T T\n"  // T = !(A && B && C)  <- hole ahead
        "NOT J J\n"  // J = true
        "AND T J\n"  // J = T
        "AND D J\n"  // J = T && D
        "WALK\n";
    MESSAGE(Survey(walk, prog));

    // Must be able to step after jump (E) or jump after jump (H)
    const char *run =
        "NOT T T\n"  // T = true
        "AND A T\n"  // T = A
        "AND B T\n"  // T = A && B
        "AND C T\n"  // T = A && B && C
        "NOT T T\n"  // T = !(A && B && C)  <- hole ahead
        "NOT J J\n"  // J = true
        "AND E J\n"  // J = E
        "OR H J\n"   // J = E || H
        "AND T J\n"  // J = T && (E || H)
        "AND D J\n"  // J = T && (E || H) && D
        "RUN\n";
    MESSAGE(Survey(run, prog));
}
