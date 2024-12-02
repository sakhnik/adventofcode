#include "../test.hpp"
#include <iterator>

namespace {

using VecT = std::vector<int>;

bool IsSafe(const VecT &levels)
{
    int order = 0;
    return std::adjacent_find(levels.begin(), levels.end(), [&](int a, int b) {
        int dist = b - a;
        if (!order)
            order = dist;
        return order > 0 ? (dist < 1 || dist > 3) : (dist < -3 || dist > -1);
    }) == levels.end();
}

std::pair<int, int> Task(std::istream &&is)
{
    int task1{}, task2{};
    std::string line;
    while (getline(is, line)) {
        using StrIterT = std::istream_iterator<int>;
        std::istringstream iss{line};
        VecT levels{StrIterT{iss}, StrIterT{}};
        if (IsSafe(levels)) {
            task1++;
            task2++;
        } else {
            VecT levels2;
            for (int i = 0; i < levels.size(); ++i) {
                levels2 = levels;
                levels2.erase(levels2.begin() + i);
                if (IsSafe(levels2)) {
                    ++task2;
                    break;
                }
            }
        }
    }
    return {task1, task2};
}

using namespace boost::ut;

suite s = [] {
    "02"_test = [] {
        const char *const TEST1 = R"(7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9
)";
        auto test = Task(std::istringstream{TEST1});
        expect(2_i == test.first);
        expect(4_i == test.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
