#include "../test.hpp"

namespace {

using namespace boost::ut;

std::pair<int, int> Task(std::istream &&is)
{
    using CodeT = std::vector<int>;
    std::vector<CodeT> locks, keys;

    std::vector<std::string> pat;
    auto processPat = [&]() {
        if (pat[0] == "#####") {
            // lock
            CodeT code;
            for (int c = 0; c < 5; ++c) {
                for (int h = 1; h <= 6; ++h) {
                    if (pat[h][c] != '#') {
                        code.push_back(h - 1);
                        break;
                    }
                }
            }
            locks.push_back(code);
        } else {
            // key
            CodeT code;
            for (int c = 0; c < 5; ++c) {
                for (int h = 5; h >= 0; --h) {
                    if (pat[h][c] != '#') {
                        code.push_back(5 - h);
                        break;
                    }
                }
            }
            keys.push_back(code);
        }
        pat.clear();
    };

    std::string line;
    while (getline(is, line)) {
        if (!line.empty()) {
            pat.push_back(line);
            continue;
        }
        processPat();
    }
    processPat();

    auto fit = [](const CodeT &key, const CodeT &lock) {
        for (int i = 0; i < 5; ++i) {
            if (key[i] + lock[i] > 5)
                return false;
        }
        return true;
    };

    int task1{};
    for (auto lock : locks) {
        for (auto key : keys) {
            if (fit(key, lock))
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
