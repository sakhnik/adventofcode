#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCode prog{ifs};

    int count{0};

    while (prog.GetState() != prog.S_HALT)
    {
        /*auto x =*/ prog.Advance(0);
        if (prog.GetState() == prog.S_HALT)
            break;
        REQUIRE(prog.GetState() == prog.S_OUTPUT);
        /*auto y =*/ prog.Advance(0);
        REQUIRE(prog.GetState() == prog.S_OUTPUT);
        auto kind = prog.Advance(0);
        if (prog.GetState() == prog.S_OUTPUT && kind == 2)
        {
            ++count;
        }
    }

    MESSAGE(count);
}
