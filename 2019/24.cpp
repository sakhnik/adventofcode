#include <doctest/doctest.h>
#include <string>
#include <bitset>
#include <unordered_set>


using BugsT = std::bitset<25>;

BugsT Parse(const std::string &input)
{
    BugsT r;
    for (int row = 0; row < 5; ++row)
        for (int col = 0; col < 5; ++col)
        {
            if (input[row * 6 + col] == '#')
                r.set(row * 5 + col);
        }
    return r;
}

std::string Dump(BugsT bugs)
{
    std::string ret;
    const char *nl = "";
    for (int r = 0; r < 5; ++r)
    {
        ret += nl;
        nl = "\n";
        for (int c = 0; c < 5; ++c)
        {
            ret.push_back(bugs.test(r * 5 + c) ? '#' : '.');
        }
    }
    return ret;
}

BugsT Evolve(BugsT bugs)
{
    bool b[7][7] = {};
    for (int r = 0; r < 5; ++r)
        for (int c = 0; c < 5; ++c)
            b[r+1][c+1] = bugs.test(r*5 + c);

    int counts[7][7] = {};
    for (int r = 1; r < 6; ++r)
        for (int c = 1; c < 6; ++c)
            if (b[r][c])
            {
                ++counts[r-1][c];
                ++counts[r+1][c];
                ++counts[r][c-1];
                ++counts[r][c+1];
            }

    for (int r = 0; r < 5; ++r)
        for (int c = 0; c < 5; ++c)
        {
            auto i = r * 5 + c;
            auto cnt = counts[r+1][c+1];
            if (bugs.test(i))
            {
                if (cnt != 1)
                    bugs.reset(i);
            }
            else
            {
                if (cnt == 1 || cnt == 2)
                    bugs.set(i);
            }
        }
    return bugs;
}

BugsT FindRepeat(BugsT bugs)
{
    std::unordered_set<BugsT> prev{bugs};
    while (true)
    {
        bugs = Evolve(bugs);
        if (prev.find(bugs) != prev.end())
        {
            return bugs;
        }
        prev.insert(bugs);
    }
}

TEST_CASE(TEST_NAME)
{
    {
        auto bugs = Parse("....#\n#..#.\n#..##\n..#..\n#....");
        bugs = Evolve(bugs);
        REQUIRE(Dump(bugs) == "#..#.\n####.\n###.#\n##.##\n.##..");
        auto b2 = FindRepeat(bugs);
        REQUIRE(2129920 == b2.to_ulong());
    }

    {
        auto bugs = Parse("..#.#\n.#.##\n##...\n.#.#.\n###..");
        auto b2 = FindRepeat(bugs);
        MESSAGE(b2.to_ulong());
    }
}
