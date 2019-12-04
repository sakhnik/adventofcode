#include <doctest/doctest.h>
#include <array>
#include <algorithm>
#include <boost/range/irange.hpp>


template <typename PredT>
bool IsPassword(int p, PredT pred)
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
    return pred(begin(digits), end(digits));
}

auto IsPassword1 = [](int p) {
    return IsPassword(p, [](auto begin, auto end) {
                          auto it = std::unique(begin, end);
                          return it - begin < end - begin;
                      });
};

template <typename IterT>
bool HaveRL2(IterT begin, IterT end)
{
    int rl{1};
    for (++begin; begin != end; ++begin)
    {
        if (*begin == *(begin - 1))
        {
            ++rl;
        }
        else
        {
            if (rl == 2)
            {
                return true;
            }
            rl = 1;
        }
    }
    return rl == 2;
}

auto IsPassword2 = [](int p) {
    return IsPassword(p, [](auto begin, auto end) {
                          return HaveRL2(begin, end);
                      });
};

template <typename PredT>
int CountPasswords(int begin, int end, PredT pred)
{
    auto r = boost::irange(begin, end);
    return std::count_if(std::begin(r), std::end(r), pred);
}

TEST_CASE(TEST_NAME)
{
    REQUIRE(IsPassword1(111111));
    REQUIRE(!IsPassword1(223450));
    REQUIRE(!IsPassword1(123789));

    MESSAGE(CountPasswords(256310, 732736, IsPassword1));

    REQUIRE(IsPassword2(112233));
    REQUIRE(!IsPassword2(123444));
    REQUIRE(IsPassword2(111122));

    MESSAGE(CountPasswords(256310, 732736, IsPassword2));
}
