#include <doctest/doctest.h>
#include <vector>

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

} //namespace;

TEST_CASE(TEST_NAME)
{
    const size_t target = 36000000;
    MESSAGE(Find(target, 10, target));
    MESSAGE(Find(target, 11, 50));
}
