#include "../test.hpp"

namespace {

template <typename IterT>
int CountIncreased(IterT first, IterT last)
{
    int count{};
    std::adjacent_find(first, last, [&count](int a, int b) { if (a < b) { ++count; } return false; });
    return count;
}

int TEST_DEPTHS[] = {199, 200, 208, 210, 200, 207, 240, 269, 260, 263};

using namespace boost::ut;

suite s = [] {
    "2021-01"_test = [] {
        expect(7_i == CountIncreased(std::begin(TEST_DEPTHS), std::end(TEST_DEPTHS)));

        std::ifstream ifs{INPUT};
        std::istream_iterator<int> first{ifs}, last;
        Printer::Print(__FILE__, "1", CountIncreased(first, last));
    };
};

} //namespace;
