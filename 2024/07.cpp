#include "../test.hpp"
#include <iterator>

namespace {

using namespace boost::ut;

using IntT = int64_t;
using NumsT = std::vector<IntT>;

bool Check(IntT target, int i, NumsT nums)
{
    if (!i)
        return target == nums[0];
    if (target % nums[i] == 0 && Check(target / nums[i], i - 1, nums))
        return true;
    if (target >= nums[i] && Check(target - nums[i], i - 1, nums))
        return true;
    return false;
}

bool Check2(IntT target, int i, NumsT nums)
{
    if (!i)
        return target == nums[0];
    if (target % nums[i] == 0 && Check2(target / nums[i], i - 1, nums))
        return true;
    if (target >= nums[i] && Check2(target - nums[i], i - 1, nums))
        return true;
    std::string st = std::to_string(target);
    std::string sn = std::to_string(nums[i]);
    if (st.ends_with(sn) && st.size() > sn.size()) {
        auto nt = st.substr(0, st.size() - sn.size());
        if (Check2(std::stoll(nt), i - 1, nums))
            return true;
    }
    return false;
}

std::pair<IntT, IntT> Task(std::istream &&is)
{
    IntT task1{}, task2{};

    std::string line;
    while (getline(is, line)) {
        std::stringstream iss{line};
        IntT target{};
        iss >> target;
        iss.get();
        NumsT nums;
        std::copy(std::istream_iterator<int>(iss), std::istream_iterator<int>{}, std::back_inserter(nums));
        if (Check(target, nums.size() - 1, nums))
            task1 += target;
        if (Check2(target, nums.size() - 1, nums))
            task2 += target;
    }

    return {task1, task2};
}

suite s = [] {
    "07"_test = [] {
        const char *const TEST1 = R"(190: 10 19
3267: 81 40 27
83: 17 5
156: 15 6
7290: 6 8 6 15
161011: 16 10 13
192: 17 8 14
21037: 9 7 18 13
292: 11 6 16 20)";
        auto test = Task(std::istringstream{TEST1});
        expect(3749_i == test.first);
        expect(11387_i == test.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
