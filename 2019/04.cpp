#include <array>
#include <algorithm>
#include <boost/range/irange.hpp>
#include <boost/ut.hpp>

namespace {

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

using namespace boost::ut;

suite s = [] {
    "2019-04"_test = [] {
        expect(IsPassword1(111111));
        expect(!IsPassword1(223450));
        expect(!IsPassword1(123789));

        std::cout << "2019-04.1: " << CountPasswords(256310, 732736, IsPassword1) << std::endl;

        expect(IsPassword2(112233));
        expect(!IsPassword2(123444));
        expect(IsPassword2(111122));

        std::cout << "2019-04.2: " << CountPasswords(256310, 732736, IsPassword2) << std::endl;
    };
};

} //namespace;