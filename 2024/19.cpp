#include "../test.hpp"

namespace {

using namespace boost::ut;

std::pair<int, int64_t> Task(std::istream &&is)
{
    int max_towel_size = 0;
    std::vector<std::string> towels;
    std::string line;
    while (getline(is, line)) {
        std::istringstream iss{line};
        std::string towel;
        while (iss >> towel) {
            if (towel.ends_with(','))
                towel.pop_back();
            towels.push_back(towel);
            if (towel.size() > max_towel_size)
                max_towel_size = towel.size();
        }
        if (line.empty())
            break;
    }

    int task1{};
    int64_t task2{};

    while (getline(is, line)) {
        std::vector<int64_t> counts(line.size() + max_towel_size, 0);
        counts[0] = 1;
        for (int i = 0; i < line.size(); ++i) {
            if (!counts[i])
                continue;
            for (const auto &t : towels) {
                if (std::string_view{line}.substr(i).starts_with(t))
                    counts[i + t.size()] += counts[i];
            }
        }

        if (counts[line.size()])
            ++task1;
        task2 += counts[line.size()];
    }

    return {task1, task2};
}

suite s = [] {
    "19"_test = [] {
        const char *const TEST1 = R"(r, wr, b, g, bwu, rb, gb, br

brwrr
bggr
gbbr
rrbgbr
ubwu
bwurrg
brgr
bbrgwb)";
        auto test = Task(std::istringstream{TEST1});
        expect(6_i == test.first);
        expect(16_i == test.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
