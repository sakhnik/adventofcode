#include "../test.hpp"

namespace {

using namespace boost::ut;

std::pair<int64_t, int> Task(std::istream &&is)
{
    std::unordered_map<uint32_t, int> bananas;

    int64_t task1{};
    int32_t n{};
    while (is >> n) {
        constexpr const int NONE = std::numeric_limits<int>::max();
        int prev = NONE;
        uint32_t key{};
        std::unordered_set<uint32_t> seller;
        for (int i = 0; i < 2000; ++i) {
            n = (n ^ (n << 6)) & 0xffffff;
            n = (n ^ (n >> 5)) & 0xffffff;
            n = (n ^ (n << 11)) & 0xffffff;

            int price = n % 10;
            if (prev != NONE) {
                int change = price - prev;
                key = (key << 8) + (change + 10);
                if (i >= 3) {
                    if (!seller.contains(key)) {
                        seller.insert(key);
                        bananas[key] += price;
                    }
                }
            }
            prev = price;
        }
        task1 += n;
    }

    int task2 = std::max_element(bananas.begin(), bananas.end(),
            [](const auto &a, const auto &b) { return a.second < b.second; })->second;

    return {task1, task2};
}

suite s = [] {
    "22"_test = [] {
        const char *const TEST1 = R"(1
10
100
2024)";
        auto test = Task(std::istringstream{TEST1});
        expect(37327623_i == test.first);

        const char *const TEST2 = R"(1
2
3
2024)";
        auto test2 = Task(std::istringstream{TEST2});
        expect(23_i == test2.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
