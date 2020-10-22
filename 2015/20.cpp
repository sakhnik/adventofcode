#include <vector>
#include "../test.hpp"
namespace {

size_t Find(size_t target, int mul, int max_repeat)
{
    target /= mul;

    std::vector<size_t> counts(target, 0);
    for (size_t i = 1, in = counts.size(); i < in; ++i)
    {
        for (size_t j = i, jn = std::min(in, max_repeat * i); j < jn; j += i)
        {
            counts[j] += i;
            if (counts[i] >= target)
                break;
        }
        if (counts[i] >= target)
            return i;
    }
    return -1;
}

using namespace boost::ut;

suite s = [] {
    "2015-20"_test = [] {
        const size_t target = 36000000;
        Printer::Print(__FILE__, "1", Find(target, 10, target));
        Printer::Print(__FILE__, "2", Find(target, 11, 50));
    };
};

} //namespace;