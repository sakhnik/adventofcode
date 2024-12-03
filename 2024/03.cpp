#include "../test.hpp"

namespace {

std::pair<int, int> Task(std::istream &&is)
{
    int task1{}, task2{};
    bool enabled{true};

    const static std::regex pattern(R"(mul\((\d+),(\d+)\)|do\(\)|don't\(\))");
    std::string line;
    while (getline(is, line)) {
        std::sregex_iterator begin{line.begin(), line.end(), pattern}, end{};
        for (auto it = begin; it != end; ++it) {
            std::smatch match = *it;
            if (match.str() == "do()") {
                enabled = true;
            } else if (match.str() == "don't()") {
                enabled = false;
            } else if (match.size() == 3) {
                auto mul = std::stoi(match.str(1)) * std::stoi(match.str(2));
                task1 += mul;
                if (enabled) {
                    task2 += mul;
                }
            }
        }
    }

    return {task1, task2};
}

using namespace boost::ut;

suite s = [] {
    "03"_test = [] {
        const char *const TEST1 = R"(xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5)))";
        expect(161_i == Task(std::istringstream{TEST1}).first);
        const char *const TEST2 = R"(xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5)))";
        expect(48_i == Task(std::istringstream{TEST2}).second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
