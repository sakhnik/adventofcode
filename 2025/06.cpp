#include <cstdint>
#include <iostream>
#include <fstream>
#include <numeric>
#include <sstream>
#include <vector>
#include <cassert>

struct Task
{
    uint64_t res1 = 0;
    uint64_t res2 = 0;

    Task(std::istream &&is)
    {
        std::vector<std::vector<uint64_t>> nums;
        std::vector<std::string> sheet;

        std::string line;
        while (std::getline(is, line)) {
            sheet.push_back(line);
            if (line[0] == '+' || line[0] == '*') {
                break;
            }
            std::istringstream iss{line};
            uint64_t num;
            int col = 0;
            while (iss >> num) {
                if (nums.size() <= col)
                    nums.emplace_back();
                nums[col++].push_back(num);
            }
        }

        std::istringstream iss{line};
        std::string op;
        int col = 0;
        while (iss >> op) {
            if (op == "+") {
                res1 += std::accumulate(nums[col].begin(), nums[col].end(), 0ull);
            } else if (op == "*") {
                res1 += std::accumulate(nums[col].begin(), nums[col].end(), 1ull,
                        [](uint64_t a, uint64_t n) {
                            return a * n;
                        });
            }
            ++col;
        }

        int height = sheet.size();
        int width = sheet[0].size();

        // Empty column will leave another 0 in the args so start with "extra column" to the right
        std::vector<uint64_t> args = { 0, 0 };
        for (int col = width - 1; col >= 0; --col) {
            bool did_op = false;
            for (int row = 0; row < height; ++row) {
                char ch = sheet[row][col];
                if (ch == '+') {
                    auto r = std::accumulate(args.begin(), args.end(), 0ull);
                    res2 += r;
                    args = { 0 };
                    did_op = true;
                } else if (ch == '*') {
                    auto r = std::accumulate(args.begin() + 1, args.end(), 1ull,
                            [](uint64_t a, uint64_t n) {
                                return a * n;
                            });
                    res2 += r;
                    args = { 0 };
                    did_op = true;
                } else if (isdigit(ch)) {
                    args.back() = args.back() * 10 + (ch - '0');
                }
            }
            if (!did_op) {
                args.push_back(0);
            }
        }
    }
};

int main()
{
    const char *const TEST =
        "123 328  51 64 \n"
        " 45 64  387 23 \n"
        "  6 98  215 314\n"
        "*   +   *   +  ";
    Task test{std::istringstream{TEST}};
    assert(test.res1 == 4277556);
    assert(test.res2 == 3263827);

    Task task{std::ifstream{"06.txt"}};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;
    return 0;
}
