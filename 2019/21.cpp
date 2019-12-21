#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCode prog{ifs};

    // The bot always jumps 4 squares ahead.
    // So it should jump as soon as a hole is detected,
    // but having the ground on the fourth tile.
    const char *script =
        "NOT A J\n"
        "NOT B T\n"
        "OR J T\n"
        "NOT C J\n"
        "OR T J\n"
        "AND D J\n"
        "WALK\n";

    int input = 0;
    while (prog.GetState() != prog.S_HALT)
    {
        auto r = prog.Advance(input);
        switch (prog.GetState())
        {
        case prog.S_RUN:
        case prog.S_HALT:
            continue;
        case prog.S_OUTPUT:
            if (r < 128)
                std::cout << static_cast<char>(r) << std::flush;
            else
                std::cout << r << std::endl;
            break;
        case prog.S_INPUT:
            input = *script++;
            break;
        }
    }
}
