#include "../test.hpp"

namespace {

uint64_t Solve()
{
    unsigned x{1}, y{1};

    uint64_t v = 20151125;
    while (true)
    {
        ++x;
        if (--y < 1)
        {
            y = x;
            x = 1;
        }
        v *= 252533;
        v %= 33554393;

        if (y == 3010 && x == 3019)
            return v;
    }
    return 0;
}

using namespace boost::ut;

suite s = [] {
    "25"_test = [] {
        Printer::Print(__FILE__, "1", Solve());
    };
};

} //namespace;
