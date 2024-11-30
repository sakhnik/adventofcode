#include "../test.hpp"
#include <queue>
#include <unordered_set>
#include <fstream>

namespace {

class Xmas
{
public:
    Xmas(size_t preamble, std::istream &&is)
    {
        std::unordered_set<uint64_t> last_nums;

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
            if (_nums.size() >= preamble)
            {
                if (!is_sum(a))
                {
                    _target = a;
                    return;
                }
                last_nums.erase(_nums[_nums.size() - preamble]);
            }
            _nums.push_back(a);
            last_nums.insert(a);
        }
    }

    uint64_t GetTarget() const { return _target; }

    uint64_t FindWeakness() const
    {
        // For every sequence that starts from i-th element.
        for (size_t i{0}; i < _nums.size(); ++i)
        {
            // Try accumulating.
            uint64_t sum = _nums[i];
            for (size_t j{i + 1}; j < _nums.size() && sum < _target; ++j)
            {
                sum += _nums[j];
                if (sum == _target)
                {
                    auto it = _nums.begin() + i;
                    auto it_end = _nums.begin() + j + 1;
                    return *std::min_element(it, it_end) + *std::max_element(it, it_end);
                }
            }
        }
        return 0;
    }

private:
    std::vector<uint64_t> _nums;
    uint64_t _target = 0;
};

using namespace boost::ut;

suite s = [] {
    "09"_test = [] {
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
            Xmas x{5, std::istringstream{TEST}};
            expect(127_u == x.GetTarget());
            expect(62_u == x.FindWeakness());
        }

        Xmas x{25, std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", x.GetTarget());
        Printer::Print(__FILE__, "2", x.FindWeakness());
    };
};

} //namespace;
