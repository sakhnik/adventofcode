#include <fstream>
#include <boost/ut.hpp>
#include "IntCode.h"

#include "../Printer.hpp"

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
        Printer::Print(__FILE__, "1", runWithNounVerb(12, 12));

        // Task 2
        for (int noun = 0; noun < 100; ++noun)
        {
            for (int verb = 0; verb < 100; ++verb)
            {
                if (19690720 == runWithNounVerb(noun, verb))
                {
                    Printer::Print(__FILE__, "2", noun * 100 + verb);
                    break;
                }
            }
        }
    };
};

} //namespace;