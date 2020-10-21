#include <vector>
#include <fstream>
#include <iterator>
#include <boost/ut.hpp>

namespace {

using LicenseT = std::vector<int>;

LicenseT ReadLicense(std::istream &&is)
{
    LicenseT lic;
    std::copy(std::istream_iterator<int>{is}, std::istream_iterator<int>{},
              std::back_inserter(lic));
    return lic;
}

template <typename IterT, typename VisitorT>
IterT Visit(IterT first, IterT last, VisitorT visitor)
{
    auto children = *first++;
    auto meta = *first++;
    for (int c = 0; c < children; ++c)
    {
        first = Visit(first, last, visitor);
    }
    for (int m = 0; m < meta; ++m)
    {
        visitor(*first++);
    }
    return first;
}

int SumMeta(const LicenseT &lic)
{
    int sum{};
    auto add = [&](int num) {
        sum += num;
    };
    Visit(begin(lic), end(lic), add);
    return sum;
}

template <typename IterT>
IterT SumValues(IterT first, IterT last, int &sum)
{
    auto children = *first++;
    auto meta = *first++;

    std::vector<int> values(children);
    for (int c = 0; c < children; ++c)
    {
        first = SumValues(first, last, values[c]);
    }

    if (!children)
    {
        for (int m = 0; m < meta; ++m)
        {
            sum += *first++;
        }
    }
    else
    {
        for (int m = 0; m < meta; ++m)
        {
            auto idx = *first++ - 1;
            if (idx >= 0 && idx < children)
            {
                sum += values[idx];
            }
        }
    }

    return first;
}

int SumValues(const LicenseT &lic)
{
    int ret{};
    SumValues(begin(lic), end(lic), ret);
    return ret;
}

using namespace boost::ut;

suite s = [] {
    "2018-08"_test = [] {
        expect(138_i == SumMeta({2, 3, 0, 3, 10, 11, 12, 1, 1, 0, 1, 99, 2, 1, 1, 2}));
        auto license = ReadLicense(std::ifstream{INPUT});
        std::cout << "2018-08.1: " << SumMeta(license) << std::endl;

        expect(66_i == SumValues({2, 3, 0, 3, 10, 11, 12, 1, 1, 0, 1, 99, 2, 1, 1, 2}));
        std::cout << "2018-08.2: " << SumValues(license) << std::endl;
    };
};

} //namespace;