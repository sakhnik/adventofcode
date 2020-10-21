#include <vector>
#include <limits>
#include <boost/ut.hpp>

#include "../Printer.hpp"

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

using namespace boost::ut;

suite s = [] {
    "2015-17.test"_test = [] {
        int sizes[] = {20, 15, 10, 5, 5};
        expect(4_i == Count(25, std::begin(sizes), std::end(sizes)));
        expect(3_i == Count2(25, std::begin(sizes), std::end(sizes)));
    };

    "2015-17.task"_test = [] {
        int sizes[] = {
            11, 30, 47, 31, 32, 36, 3, 1, 5, 3,
            32, 36, 15, 11, 46, 26, 28, 1, 19, 3
        };
        Printer::Print(__FILE__, "1", Count(150, std::begin(sizes), std::end(sizes)));
        Printer::Print(__FILE__, "2", Count2(150, std::begin(sizes), std::end(sizes)));
    };
};

} //namespace;