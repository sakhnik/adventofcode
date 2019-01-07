#include <doctest/doctest.h>
#include <vector>

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

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        int sizes[] = {20, 15, 10, 5, 5};
        REQUIRE(4 == Count(25, std::begin(sizes), std::end(sizes)));
    }

    SUBCASE("task") {
        int sizes[] = {
            11, 30, 47, 31, 32, 36, 3, 1, 5, 3,
            32, 36, 15, 11, 46, 26, 28, 1, 19, 3
        };
        MESSAGE(Count(150, std::begin(sizes), std::end(sizes)));
    }
}
