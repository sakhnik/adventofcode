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

        auto forest = Parse(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", CountVisible(forest));
    };
};

} //namespace;
