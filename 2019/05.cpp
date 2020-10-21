#include <fstream>
#include <sstream>
#include <string>
#include <boost/ut.hpp>

#include "IntCode.h"

namespace {

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2019-05"_test = [] {
        std::ifstream ifs{INPUT};
        IntCode master{ifs};

        auto run = [&](IntCode &&program, int input) {
            std::string buf;

            while (true)
            {
                auto r = program.Advance(input);
                switch (program.GetState())
                {
                case IntCode::S_HALT:
                    return buf;
                case IntCode::S_INPUT:
                    continue;
                case IntCode::S_OUTPUT:
                    if (!buf.empty())
                    {
                        buf += " ";
                    }
                    buf += std::to_string(r);
                    break;
                default:
                    break;
                }
            }
        };

        // Task 1
        std::cout << "2019-05.1: " << run(IntCode{master}, 1) << std::endl;

        // Tests
        IntCode test_eq8{std::istringstream{"3,9,8,9,10,9,4,9,99,-1,8"}};
        expect(eq("0"s, run(IntCode{test_eq8}, 1)));
        expect(eq("1"s, run(IntCode{test_eq8}, 8)));
        expect(eq("0"s, run(IntCode{test_eq8}, 10)));

        IntCode test_lt8{std::istringstream{"3,9,7,9,10,9,4,9,99,-1,8"}};
        expect(eq("1"s, run(IntCode{test_lt8}, 2)));
        expect(eq("0"s, run(IntCode{test_lt8}, 8)));
        expect(eq("0"s, run(IntCode{test_lt8}, 9)));

        IntCode test_eq8_2{std::istringstream{"3,3,1108,-1,8,3,4,3,99"}};
        expect(eq("0"s, run(IntCode{test_eq8_2}, 1)));
        expect(eq("1"s, run(IntCode{test_eq8_2}, 8)));
        expect(eq("0"s, run(IntCode{test_eq8_2}, 10)));

        IntCode test_lt8_2{std::istringstream{"3,3,1107,-1,8,3,4,3,99"}};
        expect(eq("1"s, run(IntCode{test_lt8_2}, 2)));
        expect(eq("0"s, run(IntCode{test_lt8_2}, 8)));
        expect(eq("0"s, run(IntCode{test_lt8_2}, 9)));

        IntCode test_eq0{std::istringstream{"3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9"}};
        expect(eq("0"s, run(IntCode{test_eq0}, 0)));
        expect(eq("1"s, run(IntCode{test_eq0}, -100)));
        expect(eq("1"s, run(IntCode{test_eq0}, 123)));

        // Task 2
        std::cout << "2019-05.2: " << run(IntCode{master}, 5) << std::endl;
    };
};

} //namespace;