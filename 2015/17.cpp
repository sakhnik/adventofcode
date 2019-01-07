#include <doctest/doctest.h>
#include <vector>
#include <limits>

namespace {

template <typename IterT>
int Count(int target, IterT first, IterT last)
{
    if (target == 0)
        return 1;
    if (target < 0 || first == last)
        return 0;

    return Count(target - *first, first + 1, last) + Count(target, first + 1, last);
}

class Counter
{
public:
    template <typename IterT>
    void Iterate(int target, IterT first, IterT last, int pots)
    {
        if (target == 0)
        {
            if (min_pots < pots)
                return;
            if (min_pots == pots)
                ++count;
            else
            {
                min_pots = pots;
                count = 1;
            }
            return;
        }
        if (target < 0 || first == last)
            return;
        Iterate(target - *first, first + 1, last, pots + 1);
        Iterate(target, first + 1, last, pots);
    }

    int GetCount() const { return count; }

private:
    int min_pots = std::numeric_limits<int>::max();
    int count = 0;
};

template <typename IterT>
int Count2(int target, IterT first, IterT last)
{
    Counter counter;
    counter.Iterate(target, first, last, 0);
    return counter.GetCount();
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        int sizes[] = {20, 15, 10, 5, 5};
        REQUIRE(4 == Count(25, std::begin(sizes), std::end(sizes)));
        REQUIRE(3 == Count2(25, std::begin(sizes), std::end(sizes)));
    }

    SUBCASE("task") {
        int sizes[] = {
            11, 30, 47, 31, 32, 36, 3, 1, 5, 3,
            32, 36, 15, 11, 46, 26, 28, 1, 19, 3
        };
        MESSAGE(Count(150, std::begin(sizes), std::end(sizes)));
        MESSAGE(Count2(150, std::begin(sizes), std::end(sizes)));
    }
}
