#include "../test.hpp"

namespace {

uint64_t Task1(uint64_t start, const std::vector<uint64_t> ids)
{
    uint64_t earliest_id{};
    uint64_t min_wait_time = std::numeric_limits<uint64_t>::max();

    for (size_t i = 0; i < std::size(ids); ++i)
    {
        auto cycles = start / ids[i];
        auto wait_time = (cycles + 1) * ids[i] - start;
        if (wait_time < min_wait_time)
        {
            min_wait_time = wait_time;
            earliest_id = ids[i];
        }
    }

    return min_wait_time * earliest_id;
}

using namespace boost::ut;

suite s = [] {
    "2020-13"_test = [] {


        expect(295_u == Task1(939, { 7, 13, 59, 31, 19 }));
        Printer::Print(__FILE__, "1", Task1(1000186, { 17, 37, 907, 19, 23, 29, 653, 41, 13 }));
    };
};

} //namespace
