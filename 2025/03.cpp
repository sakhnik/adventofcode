#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cassert>
#include <vector>
#include <numeric>

struct Task
{
    uint64_t res1 = 0;
    uint64_t res2 = 0;

    Task(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line)) {
            res1 += ChooseBest(line, 2);
            res2 += ChooseBest(line, 12);
        }
    }

    uint64_t ChooseBest(std::string_view s, int count)
    {
        std::vector<int> cells;
        for (int i = 0; i < s.size(); ++i) {
            int d = s[i] - '0';
            while (!cells.empty() && cells.back() < d && cells.size() + (s.size() - i) > count) {
                cells.pop_back();
            }
            if (cells.size() < count)
                cells.push_back(d);
        }
        assert(cells.size() == count);
        return std::accumulate(cells.begin(), cells.end(), 0ull,
                               [](uint64_t a, uint64_t d) {
                                    return a * 10 + d;
                               });
    }
};

int main()
{
    const char *const TEST = "987654321111111\n811111111111119\n234234234234278\n818181911112111";
    Task test{std::istringstream{TEST}};
    assert(357 == test.res1);
    assert(3121910778619ull == test.res2);

    Task task{std::ifstream{"03.txt"}};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;
    return 0;
}
