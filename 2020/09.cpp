#include "../test.hpp"
#include <queue>
#include <unordered_set>
#include <fstream>

namespace {

uint64_t FindFirst(size_t preamble, std::istream &&is)
{
    std::unordered_set<uint64_t> last_nums;
    std::queue<uint64_t> q;

    auto is_sum = [&last_nums](uint64_t num) {
        for (uint64_t i : last_nums)
        {
            uint64_t j = num - i;
            if (i != j && last_nums.contains(j))
            {
                return true;
            }
        }
        return false;
    };

    uint64_t a{};
    while (is && (is >> a))
    {
        if (q.size() == preamble)
        {
            if (!is_sum(a))
                return a;
            last_nums.erase(q.front());
            q.pop();
        }
        q.push(a);
        last_nums.insert(a);
    }

    return -1;
}

using namespace boost::ut;

suite s = [] {
    "2020-09"_test = [] {
        {
            const char *const TEST = R"(35
20
15
25
47
40
62
55
65
95
102
117
150
182
127
219
299
277
309
576)";
            expect(127_u == FindFirst(5, std::istringstream{TEST}));
        }

        Printer::Print(__FILE__, "1", FindFirst(25, std::ifstream{INPUT}));
    };
};

} //namespace;
