#include <doctest/doctest.h>
#include <fstream>
#include <sstream>

#include "IntCode.h"


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCode master{ifs};

    auto run = [&](IntCode &&program, const char *input) {
        std::istringstream iss{input};
        std::ostringstream oss;
        program.Run(iss, oss);
        return oss.str();
    };

    // Task 1
    MESSAGE(run(IntCode{master}, "1"));

    // Tests
    IntCode test_eq8{std::istringstream{"3,9,8,9,10,9,4,9,99,-1,8"}};
    REQUIRE("0" == run(IntCode{test_eq8}, "1"));
    REQUIRE("1" == run(IntCode{test_eq8}, "8"));
    REQUIRE("0" == run(IntCode{test_eq8}, "10"));

    IntCode test_lt8{std::istringstream{"3,9,7,9,10,9,4,9,99,-1,8"}};
    REQUIRE("1" == run(IntCode{test_lt8}, "2"));
    REQUIRE("0" == run(IntCode{test_lt8}, "8"));
    REQUIRE("0" == run(IntCode{test_lt8}, "9"));

    IntCode test_eq8_2{std::istringstream{"3,3,1108,-1,8,3,4,3,99"}};
    REQUIRE("0" == run(IntCode{test_eq8_2}, "1"));
    REQUIRE("1" == run(IntCode{test_eq8_2}, "8"));
    REQUIRE("0" == run(IntCode{test_eq8_2}, "10"));

    IntCode test_lt8_2{std::istringstream{"3,3,1107,-1,8,3,4,3,99"}};
    REQUIRE("1" == run(IntCode{test_lt8_2}, "2"));
    REQUIRE("0" == run(IntCode{test_lt8_2}, "8"));
    REQUIRE("0" == run(IntCode{test_lt8_2}, "9"));

    IntCode test_eq0{std::istringstream{"3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9"}};
    REQUIRE("0" == run(IntCode{test_eq0}, "0"));
    REQUIRE("1" == run(IntCode{test_eq0}, "-100"));
    REQUIRE("1" == run(IntCode{test_eq0}, "123"));


    // Task 2
    MESSAGE(run(IntCode{master}, "5"));
}
