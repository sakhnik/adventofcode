#include "../test.hpp"
#include <fstream>

namespace {

struct Sequence
{
    std::vector<int> nums;
    std::vector<int> next, prev;
    int start{};

    Sequence(std::istream &&is)
    {
        int num{};
        while (is >> num)
            nums.push_back(num);

        // Form a circular list
        next.resize(nums.size());
        std::iota(next.begin(), next.end(), 1);
        next.back() = 0;
        prev.resize(nums.size());
        std::iota(prev.begin(), prev.end(), -1);
        prev.front() = nums.size() - 1;
    }

    void Mix(int i, int64_t key)
    {
        int num = (key * nums[i]) % static_cast<int>(nums.size() - 1);
        if (!num)
            return;
        if (num < 0)
            --num;

        // delete at i
        next[prev[i]] = next[i];
        prev[next[i]] = prev[i];

        if (i == start && num != -1)
            start = next[start];

        int target_i{i};
        while (num > 0)
        {
            target_i = next[target_i];
            --num;
        }
        while (num < 0)
        {
            target_i = prev[target_i];
            ++num;
        }

        // insert at target_i
        prev[i] = target_i;
        prev[next[target_i]] = i;
        next[i] = next[target_i];
        next[target_i] = i;
    }

    int64_t Decrypt(int64_t key, int repeats)
    {
        while (repeats--)
            for (int i = 0; i < nums.size(); ++i)
                Mix(i, key);

        int64_t sum{};
        int i = std::find(nums.begin(), nums.end(), 0) - nums.begin();
        for (int k = 0; k < 3; ++k)
        {
            for (int j = 0, n = 1000 % nums.size(); j < n; ++j)
                i = next[i];
            sum += nums[i];
        }
        return key * sum;
    }

    int Task1()
    {
        return Decrypt(1, 1);
    }

    int64_t Task2()
    {
        return Decrypt(811589153LL, 10);
    }

    void Print() const
    {
        int i = start;
        do
        {
            std::cout << " " << nums[i];
            i = next[i];
        }
        while (i != start);
        std::cout << std::endl;
    }
};

const char *const TEST = R"(1
2
-3
3
-2
0
4)";

using namespace boost::ut;

suite s = [] {
    "20"_test = [] {
        Sequence test{std::istringstream{TEST}};
        expect(3_i == test.Task1());
        Sequence test2{std::istringstream{TEST}};
        expect(1623178306_i == test2.Task2());

        Sequence sequence{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", sequence.Task1());
        Sequence sequence2{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "2", sequence2.Task2());
    };
};

} //namespace;
