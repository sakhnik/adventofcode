#include "../test.hpp"

namespace {

using namespace boost::ut;

std::pair<int, int> Task(std::istream &&is)
{
    using PatT = std::vector<std::string>;
    std::vector<PatT> patterns;

    PatT pat;
    std::string line;
    while (getline(is, line)) {
        if (!line.empty()) {
            pat.push_back(line);
            continue;
        }
        patterns.push_back(pat);
        pat.clear();
    }
    patterns.push_back(pat);

    auto fit = [](const PatT &a, const PatT &b) {
        for (int i = 0; i < a.size(); ++i) {
            for (int j = 0; j < a[0].size(); ++j) {
                if (a[i][j] == '#' && b[i][j] == '#')
                    return false;
            }
        }
        return true;
    };

    int task1{};
    for (int i = 1; i < patterns.size(); ++i) {
        for (int j = 0; j < i; ++j) {
            if (fit(patterns[i], patterns[j]))
                ++task1;
        }
    }

    return {task1, 0};
}

suite s = [] {
    "25"_test = [] {
        const char *const TEST1 = R"(#####
.####
.####
.####
.#.#.
.#...
.....

#####
##.##
.#.##
...##
...#.
...#.
.....

.....
#....
#....
#...#
#.#.#
#.###
#####

.....
.....
#.#..
###..
###.#
###.#
#####

.....
.....
.....
#....
#.#..
#.#.#
#####)";
        auto test = Task(std::istringstream{TEST1});
        expect(3_i == test.first);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        //Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
