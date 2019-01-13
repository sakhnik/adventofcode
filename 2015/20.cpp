#include <doctest/doctest.h>
#include <vector>

namespace {

int Find(int target)
{
    target /= 10;

    std::vector<int> counts(target, 0);
    for (int i = 1, in = counts.size(); i < in; ++i)
    {
        for (int j = i; j < in; j += i)
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
    MESSAGE(Find(36000000));
}
