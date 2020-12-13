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

struct Bus
{
    uint64_t num, rem;
};

uint64_t Task2(const std::vector<Bus> buses)
{
    uint64_t prod = buses[0].num;
    uint64_t res = buses[0].rem;

    // Chinese remainder theorem, searching by sieving (Wikipedia).
    for (size_t i = 1; i < buses.size(); ++i)
    {
        uint64_t m = buses[i].num;
        uint64_t r = m - buses[i].rem % m;
        while (res % m != r)
            res += prod;
        prod *= m;
    }

    return res;
}

using namespace boost::ut;

suite s = [] {
    "2020-13"_test = [] {
        expect(295_u == Task1(939, { 7, 13, 59, 31, 19 }));
        Printer::Print(__FILE__, "1", Task1(1000186, { 17, 37, 907, 19, 23, 29, 653, 41, 13 }));

        expect(3417_u == Task2({{17,0}, {13,2}, {19,3}}));
        expect(754018_u == Task2({{67,0}, {7,1}, {59,2}, {61,3}}));
        expect(779210_u == Task2({{67,0}, {7,2}, {59,3}, {61,4}}));
        expect(1261476_u == Task2({{67,0}, {7,1}, {59,3}, {61,4}}));
        expect(1202161486_u == Task2({{1789,0}, {37,1}, {47,2}, {1889,3}}));
        Printer::Print(__FILE__, "2",
                       Task2({{17,0}, {37,11}, {907,17}, {19,29}, {23,40}, {29,46}, {653,48}, {41,58}, {13,61}}));
    };
};

} //namespace
