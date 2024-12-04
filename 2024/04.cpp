#include "../test.hpp"

namespace {

using WordsT = std::vector<std::string>;

bool Find(const WordsT &words, int row, int col, int dr, int dc, const char *word)
{
    if (!*word)
        return true;
    if (row < 0 || row >= words.size()) return false;
    if (col < 0 || col >= words[0].size()) return false;
    if (words[row][col] != *word) return false;
    return Find(words, row + dr, col + dc, dr, dc, word + 1);
}

std::pair<int, int> Task(std::istream &&is)
{
    WordsT words;
    std::string line;
    while (getline(is, line)) {
        words.emplace_back(std::move(line));
    }

    int task1{};
    for (int row = 0; row < words.size(); ++row) {
        for (int col = 0; col < words[0].size(); ++col) {
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr || dc) {
                        if (Find(words, row, col, dr, dc, "XMAS")) {
                            ++task1;
                        }
                    }
                }
            }
        }
    }

    int task2{};
    for (int r = 0; r < words.size() - 2; ++r) {
        for (int c = 0; c < words[0].size() - 2; ++c) {
            std::string pat = {
                words[r][c], words[r][c+2],
                words[r+1][c+1],
                words[r+2][c], words[r+2][c+2]
            };
            if (pat == "MSAMS" || pat == "SSAMM" || pat == "SMASM" || pat == "MMASS") {
                ++task2;
            }
        }
    }
    return {task1, task2};
}

using namespace boost::ut;

suite s = [] {
    "04"_test = [] {
        const char *const TEST1 = R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX)";
        auto test = Task(std::istringstream{TEST1});
        expect(18_i == test.first);
        //const char *const TEST2 = R"(xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5)))";
        expect(9_i == test.second);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
