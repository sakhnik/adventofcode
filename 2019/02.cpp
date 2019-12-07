#include <doctest/doctest.h>
#include <fstream>
#include <iostream>
#include "IntCode.h"


TEST_CASE(TEST_NAME)
{
    // Input the memory from the file
    std::ifstream ifs{INPUT};
    IntCode master{ifs};

    auto runWithNounVerb = [&](int noun, int verb) {
        IntCode task{master};
        task.SetMemory(1, noun);
        task.SetMemory(2, verb);
        task.Advance(0);
        return task.GetMemory(0);
    };

    // Task 1
    MESSAGE(runWithNounVerb(12, 12));

    // Task 2
    for (int noun = 0; noun < 100; ++noun)
    {
        for (int verb = 0; verb < 100; ++verb)
        {
            if (19690720 == runWithNounVerb(noun, verb))
            {
                MESSAGE(noun * 100 + verb);
                break;
            }
        }
    }
}
