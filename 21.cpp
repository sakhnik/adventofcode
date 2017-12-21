#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>

typedef std::unordered_map<std::string, std::string> PatternsT;

void RemoveSlashes(std::string &s)
{
    s.erase(std::remove(s.begin(), s.end(), '/'), s.end());
}

std::set<std::string> GetVariations(std::string pattern)
{
    unsigned size = 0;
    if (pattern.size() == 4)
        size = 2;
    else if (pattern.size() == 9)
        size = 3;
    else
        REQUIRE(false);

    auto get_symmetric = [=](const std::string &p) {
        auto ret(p);
        for (unsigned r = 0; r < size; ++r)
        {
            for (unsigned c = 0; c < size; ++c)
                ret[c*size + r] = p[r*size + c];
        }
        return ret;
    };

    auto get_flipped = [=](const std::string &p) {
        auto ret(p);
        for (unsigned r = 0; r < size; ++r)
        {
            for (unsigned c = 0; c < size; ++c)
                ret[r*size + c] = p[r*size + size - c - 1];
        }
        return ret;
    };

    std::set<std::string> ret;
    ret.insert(pattern);
    for (int i = 0; i < 4; ++i)
    {
        pattern = get_symmetric(pattern);
        ret.insert(pattern);
        pattern = get_flipped(pattern);
        ret.insert(pattern);
    }

    return ret;
}

PatternsT Parse(std::istream &is)
{
    PatternsT patterns;
    std::string line;
    while (getline(is, line))
    {
        RemoveSlashes(line);
        std::string pattern, skip, result;
        std::istringstream(line) >> pattern >> skip >> result;
        for (const auto &p : GetVariations(pattern))
            patterns[p] = result;
    }
    return patterns;
}

typedef std::vector<std::string> GridT;

std::string ReadBlock(unsigned row, unsigned col, unsigned size, const GridT &grid)
{
    std::string ret;
    for (unsigned r = row * size, n = r + size; r < n; ++r)
    {
        for (unsigned c = col * size, m = c + size; c < m; ++c)
        {
            ret.push_back(grid[r][c]);
        }
    }
    return ret;
}

void WriteBlock(const std::string &block, unsigned row, unsigned col, unsigned size, GridT &grid)
{
    for (unsigned r = 0; r < size; ++r)
    {
        for (unsigned c = 0; c < size; ++c)
        {
            grid[row * size + r][col * size + c] = block[r*size + c];
        }
    }
}

GridT Transform(const GridT &grid, const PatternsT &patterns)
{
    unsigned block_size = 3;
    if (grid.size() % 2 == 0)
        block_size = 2;
    unsigned new_size = grid.size() / block_size * (block_size + 1);
    GridT new_grid(new_size);
    for (auto &r : new_grid)
        r.resize(new_size, '.');

    for (unsigned row = 0, n = grid.size() / block_size; row < n; ++row)
    {
        for (unsigned col = 0; col < n; ++col)
        {
            auto block = ReadBlock(row, col, block_size, grid);
            auto it = patterns.find(block);
            if (it == patterns.end())
            {
                std::cerr << "Unknown pattern " << block << std::endl;
                REQUIRE(false);
            }
            WriteBlock(it->second, row, col, block_size + 1, new_grid);
        }
    }

    return new_grid;
}

void Print(const GridT &grid)
{
    for (const auto &r : grid)
    {
        std::cout << r << "\n";
    }
    std::cout << std::endl;
}

unsigned Solve1(const PatternsT &patterns)
{
    GridT grid = { ".#.", "..#", "###" };
    for (int i = 0; i < 5; ++i)
    {
        grid = Transform(grid, patterns);
    }

    unsigned count{0};
    for (const auto &row : grid)
        count += std::count(row.begin(), row.end(), '#');
    return count;
}

TEST_CASE("GetVariations2")
{
    auto v = GetVariations("####");
    CHECK(v.size() == 1);
    CHECK(v.count("####") == 1);

    v = GetVariations("#...");
    CHECK(v.size() == 4);
    CHECK(v.count("#...") == 1);
    CHECK(v.count(".#..") == 1);
    CHECK(v.count("..#.") == 1);
    CHECK(v.count("...#") == 1);

    v = GetVariations("##..");
    CHECK(v.size() == 4);
    CHECK(v.count("##..") == 1);
    CHECK(v.count("..##") == 1);
    CHECK(v.count("#.#.") == 1);
    CHECK(v.count(".#.#") == 1);
}

TEST_CASE("1")
{
    std::istringstream iss("../.# => ##./#../...\n.#./..#/### => #..#/..../..../#..#");
    auto patterns = Parse(iss);
    GridT grid = { ".#.", "..#", "###" };
    Print(grid);
    grid = Transform(grid, patterns);
    Print(grid);
    grid = Transform(grid, patterns);
    Print(grid);
}

TEST_CASE("main")
{
    std::ifstream ifs(INPUT);
    auto patterns = Parse(ifs);
    std::cout << Solve1(patterns) << std::endl;
}
