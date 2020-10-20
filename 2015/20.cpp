#include <vector>
#include <boost/ut.hpp>

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
        std::cout << "2015-20.1: " << Find(target, 10, target) << std::endl;
        std::cout << "2015-20.2: " << Find(target, 11, 50) << std::endl;
    };
};

} //namespace;