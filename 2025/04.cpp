#include <iostream>
#include <fstream>
#include <unordered_set>
#include <vector>
#include <numeric>
#include <boost/functional/hash.hpp>

struct Task
{
    int res1 = 0;
    int res2 = 0;

    //Task(std::istream &&is)
    //{
    //    struct Pos
    //    {
    //        int row, col;

    //        bool operator==(const Pos &o) const = default;

    //        struct Hash
    //        {
    //            size_t operator()(const Pos &p) const
    //            {
    //                size_t h = p.row;
    //                boost::hash_combine(h, p.col);
    //                return h;
    //            }
    //        };
    //    };
    //    std::unordered_set<Pos, Pos::Hash> rolls;

    //    std::string line;
    //    int height = 0, width = 0;
    //    while (std::getline(is, line)) {
    //        width = std::max(width, int(line.size()));
    //        for (int col = 0; col < line.size(); ++col) {
    //            if (line[col] == '@')
    //                rolls.emplace(height, col);
    //        }
    //        ++height;
    //    }

    //    auto remove = [&]() {
    //        decltype(rolls) rolls2;
    //        for (auto [row, col] : rolls) {
    //            int count = -1;
    //            for (int dr = -1; dr <= 1; ++dr) {
    //                for (int dc = -1; dc <= 1; ++dc) {
    //                    count += rolls.contains({row + dr, col + dc});
    //                }
    //            }
    //            if (count >= 4)
    //                rolls2.emplace(row, col);
    //        }
    //        rolls.swap(rolls2);
    //        return rolls2.size() - rolls.size();
    //    };

    //    std::vector<int> counts;
    //    for (int count = remove(); count; count = remove()) {
    //        counts.push_back(count);
    //    }

    //    res1 = counts[0];
    //    res2 = std::accumulate(counts.begin(), counts.end(), 0);
    //}

    Task(std::istream &&is)
    {
        std::vector<std::string> rolls;

        std::string line;
        while (std::getline(is, line)) {
            rolls.emplace_back(std::move(line));
        }

        int height = rolls.size();
        int width = rolls.front().size();

        auto has_roll = [&](int row, int col) -> bool {
            return row >= 0 && col >= 0 && row < height && col < width && rolls[row][col] == '@';
        };

        auto rolls2 = rolls;

        auto remove = [&]() {
            int remove_count = 0;
            for (int row = 0; row < height; ++row) {
                for (int col = 0; col < width; ++col) {
                    if (!has_roll(row, col)) {
                        rolls2[row][col] = '.';
                        continue;
                    }
                    int count = -1;
                    for (int dr = -1; dr <= 1; ++dr) {
                        for (int dc = -1; dc <= 1; ++dc) {
                            count += has_roll(row + dr, col + dc);
                        }
                    }
                    if (count < 4) {
                        rolls2[row][col] = '.';
                        ++remove_count;
                    } else {
                        rolls2[row][col] = '@';
                    }
                }
            }
            rolls.swap(rolls2);
            return remove_count;
        };

        std::vector<int> counts;
        for (int count = remove(); count; count = remove()) {
            counts.push_back(count);
        }

        res1 = counts[0];
        res2 = std::accumulate(counts.begin(), counts.end(), 0);
    }
};

int main()
{
    const char *const TEST =
        "..@@.@@@@.\n"
        "@@@.@.@.@@\n"
        "@@@@@.@.@@\n"
        "@.@@@@..@.\n"
        "@@.@@@@.@@\n"
        ".@@@@@@@.@\n"
        ".@.@.@.@@@\n"
        "@.@@@.@@@@\n"
        ".@@@@@@@@.\n"
        "@.@.@@@.@.\n";
    Task test{std::istringstream{TEST}};
    assert(13 == test.res1);
    assert(43 == test.res2);

    Task task{std::ifstream{"04.txt"}};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;
    return 0;
}
