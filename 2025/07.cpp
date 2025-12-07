#include <cstdint>
#include <unordered_map>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

struct Task
{
    uint64_t res1 = 0;
    uint64_t res2 = 0;

    Task(std::istream &&is)
    {
        using CountsT = std::unordered_map<int, uint64_t>;
        CountsT counts;

        std::string line;
        while (std::getline(is, line)) {
            CountsT next_counts;

            if (counts.empty()) {
                counts[line.find('S')] = 1;
                continue;
            }

            for (auto [col, count] : counts) {
                if (line[col] == '^') {
                    ++res1;

                    auto left = col - 1;
                    auto right = col + 1;

                    next_counts[left] += count;
                    next_counts[right] += count;
                } else {
                    next_counts[col] += count;
                }
            }

            counts.swap(next_counts);
        }

        res2 = std::accumulate(counts.begin(), counts.end(), 0ull,
                [](uint64_t a, auto &v) { return a + v.second; });
    }
};

int main()
{
    const char *const TEST =
        ".......S.......\n"
        "...............\n"
        ".......^.......\n"
        "...............\n"
        "......^.^......\n"
        "...............\n"
        ".....^.^.^.....\n"
        "...............\n"
        "....^.^...^....\n"
        "...............\n"
        "...^.^...^.^...\n"
        "...............\n"
        "..^...^.....^..\n"
        "...............\n"
        ".^.^.^.^.^...^.\n"
        "...............\n";

    Task test{std::istringstream{TEST}};
    assert(test.res1 == 21);
    assert(test.res2 == 40);

    Task task{std::ifstream{"07.txt"}};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;

    return 0;
}
