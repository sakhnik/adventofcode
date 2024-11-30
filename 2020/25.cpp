#include "../test.hpp"

namespace {

using namespace boost::ut;

uint64_t Transform(uint64_t subj, uint64_t val)
{
    val *= subj;
    val %= 20201227;
    return val;
}

uint64_t TransformLoop(int count, uint64_t subj)
{
    uint64_t val = subj;
    while (count-- > 1)
        val = Transform(val, subj);
    return val;
}

size_t FindLoopSize(uint64_t target)
{
    uint64_t val = 7;
    for (size_t i = 2; ; ++i)
    {
        val = Transform(7, val);
        if (val == target)
            return i;
    }
    return -1;
}

suite s = [] {
    "25"_test = [] {
        expect(8_u == FindLoopSize(5764801));
        expect(11_u == FindLoopSize(17807724));
        expect(14897079_u == TransformLoop(8, 17807724));

        size_t loop = FindLoopSize(8335663);
        uint64_t key = TransformLoop(loop, 8614349);
        Printer::Print(__FILE__, "1", key);
    };
};

} //namespace;
