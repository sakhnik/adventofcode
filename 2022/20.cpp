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

    void Mix(int i)
    {
        int num = nums[i] % static_cast<int>(nums.size() - 1);
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

    int Task1()
    {
        for (int i = 0; i < nums.size(); ++i)
            Mix(i);

        int sum{};
        int i = std::find(nums.begin(), nums.end(), 0) - nums.begin();
        for (int k = 0; k < 3; ++k)
        {
            for (int j = 0, n = 1000 % nums.size(); j < n; ++j)
                i = next[i];
            sum += nums[i];
        }
        return sum;
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
    "2022-20"_test = [] {
        Sequence test{std::istringstream{TEST}};
        expect(3_i == test.Task1());

        Sequence sequence{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", sequence.Task1());
    };
};

} //namespace;
