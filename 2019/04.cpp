#include <doctest/doctest.h>
#include <array>
#include <algorithm>


bool IsPassword(int p)
{
    std::array<char, 6> digits;
    for (int i = 0; i < 6; ++i)
    {
        digits[digits.size() - i - 1] = p % 10;
        p /= 10;
    }
    if (!std::is_sorted(begin(digits), end(digits)))
    {
        return false;
    }
    auto it = std::adjacent_find(begin(digits), end(digits),
                                 [](char a, char b) { return a == b; });
    return it != end(digits);
}

int CountPasswords(int begin, int end)
{
    int count{};

    for (int i = begin; i <= end; ++i)
    {
        if (IsPassword(i))
        {
            ++count;
        }
    }

    return count;
}

TEST_CASE(TEST_NAME)
{
    REQUIRE(IsPassword(111111));
    REQUIRE(!IsPassword(223450));
    REQUIRE(!IsPassword(123789));

    MESSAGE(CountPasswords(256310, 732736));
}
