#include <doctest/doctest.h>
#include "IntCode.h"
#include <boost/multiprecision/gmp.hpp>
#include <fstream>


using IntT = boost::multiprecision::mpz_int;
using IntCodeB = IntCodeImpl<IntT>;

template<>
int to_int(IntT v)
{
    return v.convert_to<int>();
}

TEST_CASE(TEST_NAME)
{
    {
        const char *t = "109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99";
        IntCodeB p{std::istringstream{t}};
        int res[] = {109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99};
        for (int r : res)
        {
            REQUIRE(r == p.Advance(0));
            REQUIRE(p.GetState() == p.S_OUTPUT);
        }
        p.Advance(0);
        REQUIRE(p.GetState() == p.S_HALT);
    }

    {
        IntCodeB p{std::istringstream{"1102,34915192,34915192,7,4,7,99,0"}};
        auto r = p.Advance(0);
        REQUIRE(p.GetState() == p.S_OUTPUT);
        REQUIRE(r.str().size() == 16);
    }

    std::ifstream ifs{INPUT};
    IntCodeB program{ifs};

    {
        IntCodeB t{program};
        auto r = t.Advance(0);
        REQUIRE(t.GetState() == t.S_INPUT);
        r = t.Advance(1);
        REQUIRE(t.GetState() == t.S_OUTPUT);
        std::cout << r << std::endl;
    }

    {
        IntCodeB t{program};
        auto r = t.Advance(0);
        REQUIRE(t.GetState() == t.S_INPUT);
        r = t.Advance(2);
        REQUIRE(t.GetState() == t.S_OUTPUT);
        std::cout << r << std::endl;
    }
}
