#include "../test.hpp"
#include <fstream>

namespace {

using ForestT = std::vector<std::string>;

ForestT Parse(std::istream &&is)
{
    ForestT forest;
    std::string line;
    while (std::getline(is, line))
        forest.push_back(std::move(line));
    return forest;
}

int CountVisible(const ForestT &forest)
{
    auto idx = [](int row, int col) {
        return (row << 16) | col;
    };
    std::unordered_set<int> visible;

    for (int row = 0; row < forest.size(); ++row)
    {
        char level = '0' - 1;
        for (int col = 0; col < forest[0].size(); ++col)
        {
            if (forest[row][col] > level)
            {
                visible.insert(idx(row, col));
                level = forest[row][col];
            }
            if (level == '9')
                break;
        }
    }

    for (int col = 0; col < forest[0].size(); ++col)
    {
        char level = '0' - 1;
        for (int row = 0; row < forest.size(); ++row)
        {
            if (forest[row][col] > level)
            {
                visible.insert(idx(row, col));
                level = forest[row][col];
            }
            if (level == '9')
                break;
        }
    }

    for (int row = 0; row < forest.size(); ++row)
    {
        char level = '0' - 1;
        for (int col = forest[0].size() - 1; col >= 0; --col)
        {
            if (forest[row][col] > level)
            {
                visible.insert(idx(row, col));
                level = forest[row][col];
            }
            if (level == '9')
                break;
        }
    }

    for (int col = 0; col < forest[0].size(); ++col)
    {
        char level = '0' - 1;
        for (int row = forest.size() - 1; row >= 0; --row)
        {
            if (forest[row][col] > level)
            {
                visible.insert(idx(row, col));
                level = forest[row][col];
            }
            if (level == '9')
                break;
        }
    }

    return visible.size();
}

size_t GetScenicScore(const ForestT &forest, int row, int col)
{
    size_t score{1};

    int r{row};
    for (r = row - 1; r >= 0; --r)
    {
        if (forest[r][col] >= forest[row][col])
        {
            score *= row - r;
            break;
        }
    }
    if (r < 0)
        score *= row;

    for (r = row + 1; r < forest.size(); ++r)
    {
        if (forest[r][col] >= forest[row][col])
        {
            score *= r - row;
            break;
        }
    }
    if (r >= forest.size())
        score *= forest.size() - 1 - row;

    int c{col};
    for (c = col - 1; c >= 0; --c)
    {
        if (forest[row][c] >= forest[row][col])
        {
            score *= col - c;
            break;
        }
    }
    if (c < 0)
        score *= col;

    for (c = col + 1; c < forest[0].size(); ++c)
    {
        if (forest[row][c] >= forest[row][col])
        {
            score *= c - col;
            break;
        }
    }
    if (c >= forest[0].size())
        score *= forest[0].size() - 1 - col;

    return score;
}

size_t GetHighestScenicScore(const ForestT &forest)
{
    size_t max_score{0};
    for (int row = 0; row < forest.size(); ++row)
    {
        for (int col = 0; col < forest[0].size(); ++col)
        {
            auto score = GetScenicScore(forest, row, col);
            max_score = std::max(score, max_score);
        }
    }
    return max_score;
}

const char *const TEST =
    "30373\n"
    "25512\n"
    "65332\n"
    "33549\n"
    "35390\n";

using namespace boost::ut;

suite s = [] {
    "2022-08"_test = [] {
        auto test = Parse(std::istringstream{TEST});
        expect(21_i == CountVisible(test));
        expect(4_u == GetScenicScore(test, 1, 2));
        expect(8_u == GetScenicScore(test, 3, 2));
        expect(8_u == GetHighestScenicScore(test));

        auto forest = Parse(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", CountVisible(forest));
        Printer::Print(__FILE__, "2", GetHighestScenicScore(forest));
    };
};

} //namespace;
