#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

class Mirage
{
public:
    using NumsT = std::vector<int>;

    int sum{}, backward{};

    Mirage(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            std::istringstream iss{line};
            NumsT nums;
            int num{};
            while (iss >> num)
                nums.push_back(num);
            auto res = Extrapolate(nums);
            sum += res.first;
            backward += res.second;
        }
    }

    int Task1() const
    {
        return sum;
    }

    int Task2() const
    {
        return backward;
    }

    std::pair<int, int> Extrapolate(const NumsT &nums)
    {
        std::vector<NumsT> difs;
        difs.push_back(nums);
        while (true)
        {
            difs.push_back(NumsT(difs.back().size() - 1, 0));
            auto &cur = difs.back();
            auto &prev = difs[difs.size() - 2];
            for (int i = 0; i < cur.size(); ++i)
                cur[i] = prev[i + 1] - prev[i];
            if (std::adjacent_find(cur.begin(), cur.end(), std::not_equal_to<>()) == cur.end())
                break;
        }
        int forward{0};
        int backward{0};
        for (auto it = difs.rbegin(); it != difs.rend(); ++it)
        {
            forward += it->back();
            backward = (*it)[0] - backward;
        }
        return {forward, backward};
    }
};

suite s = [] {
    "09"_test = [] {
        const char *const TEST1 = R"(0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45
)";
        Mirage test_mirage{std::istringstream{TEST1}};
        expect(114_i == test_mirage.Task1());
        expect(2_i == test_mirage.Task2());

        Mirage mirage{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", mirage.Task1());
        Printer::Print(__FILE__, "2", mirage.Task2());
    };
};

} //namespace;
