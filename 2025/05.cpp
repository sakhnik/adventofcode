#include <cstdint>
#include <utility>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>

struct Task
{
    uint64_t res1{}, res2{};

    using RangeT = std::pair<uint64_t, uint64_t>;
    using RangesT = std::vector<RangeT>;

    Task(std::istream &&is)
    {
        RangesT ranges;

        std::string line;
        while (std::getline(is, line)) {
            if (line.empty())
                break;
            auto dash = line.find('-');
            ranges.emplace_back(std::stoull(line.substr(0, dash)),
                                std::stoull(line.substr(dash + 1)));
        }

        Merge(ranges);

        res2 = std::accumulate(ranges.begin(), ranges.end(), 0ull,
            [](uint64_t a, const RangeT &r) {
                return a + r.second - r.first + 1;
            });

        while (std::getline(is, line)) {
            res1 += Contains(std::stoull(line), ranges);
        }
    }

    void Merge(RangesT &ranges)
    {
        std::sort(ranges.begin(), ranges.end());

        auto cur = ranges.begin();
        for (auto it = std::next(ranges.begin()); it != ranges.end(); ++it) {
            if (it->first <= cur->second + 1) {
                cur->second = std::max(cur->second, it->second);
            } else {
                ++cur;
                *cur = *it;
            }
        }

        ranges.erase(++cur, ranges.end());
    }

    bool Contains(uint64_t id, const RangesT &ranges)
    {
        for (const auto &range : ranges) {
            if (range.first <= id && id <= range.second)
                return true;
        }
        return false;
        //auto it = std::upper_bound(ranges.begin(), ranges.end(), id,
        //    [](uint64_t v, const auto& range) {
        //        return v < range.first;
        //    });
        //if (it == ranges.begin())
        //    return false;
        //--it;
        //return it->first <= id && id <= it->second;
    }
};

int main()
{
    const char *const TEST =
        "3-5\n"
        "10-14\n"
        "16-20\n"
        "12-18\n"
        "\n"
        "1\n"
        "5\n"
        "8\n"
        "11\n"
        "17\n"
        "32\n";
    Task test{std::istringstream{TEST}};
    assert(test.res1 == 3);
    assert(test.res2 == 14);

    Task task{std::ifstream{"05.txt"}};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;
    return 0;
}
