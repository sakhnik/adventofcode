#include "../test.hpp"
#include <fstream>
#include <iterator>
#include <vector>

namespace {

template <typename IterT>
int CountIncreased(int window_size, IterT first, IterT last)
{
    int sum{};  // Sum of depths in the sliding window
    IterT i{first};  // Sliding window end iterator
    // Preroll
    while (window_size-- && i != last)
        sum += *i++;

    int prev_sum = sum;
    int count{};
    while (i != last)
    {
        // Update the sliding window sum
        sum += *i++ - *first++;
        if (sum > prev_sum)
            ++count;
        prev_sum = sum;
    }
    return count;
}

int TEST_DEPTHS[] = {199, 200, 208, 210, 200, 207, 240, 269, 260, 263};

using namespace boost::ut;

suite s = [] {
    "2021-01"_test = [] {
        expect(7_i == CountIncreased(1, std::begin(TEST_DEPTHS), std::end(TEST_DEPTHS)));
        expect(5_i == CountIncreased(3, std::begin(TEST_DEPTHS), std::end(TEST_DEPTHS)));

        std::ifstream ifs{INPUT};
        std::istream_iterator<int> first{ifs}, last;
        std::vector<int> depths{first, last};
        Printer::Print(__FILE__, "1", CountIncreased(1, depths.begin(), depths.end()));
        Printer::Print(__FILE__, "2", CountIncreased(3, depths.begin(), depths.end()));
    };
};

} //namespace;
