#include <fstream>
#include <iostream>
#include <boost/ut.hpp>
#include "IntCode.h"

namespace {

using namespace boost::ut;

suite s = [] {
    "2019-02"_test = [] {
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
        std::cout << "2019-02.1: " << runWithNounVerb(12, 12) << std::endl;

        // Task 2
        for (int noun = 0; noun < 100; ++noun)
        {
            for (int verb = 0; verb < 100; ++verb)
            {
                if (19690720 == runWithNounVerb(noun, verb))
                {
                    std::cout << "2019-02.2: " << noun * 100 + verb << std::endl;
                    break;
                }
            }
        }
    };
};

} //namespace;