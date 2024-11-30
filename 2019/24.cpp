#include <string>
#include <bitset>
#include <unordered_set>
#include <map>
#include <vector>
#include <cassert>
#include <numeric>
#include "../test.hpp"

namespace {

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

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "24"_test = [] {
        {
            auto bugs = Parse("....#\n#..#.\n#..##\n..#..\n#....");
            bugs = Evolve(bugs);
            expect(eq(Dump(bugs), "#..#.\n####.\n###.#\n##.##\n.##.."s));
            auto b2 = FindRepeat(bugs);
            expect(2129920_u == b2.to_ulong());
        }

        {
            auto bugs = Parse("..#.#\n.#.##\n##...\n.#.#.\n###..");
            auto b2 = FindRepeat(bugs);
            Printer::Print(__FILE__, "1", b2.to_ulong());
        }

        {
            using LevelT = std::unordered_set<int>;
            using LevelsT = std::map<int, LevelT>;

            auto getIdx = [](int r, int c) {
                return r * 5 + c;
            };

            auto evolve = [&](const LevelsT &levels) {
                auto getCountImpl = [&](int l, int r, int c, int dir) -> int {
                    if (r == 2 && c == 2)
                    {
                        auto it = levels.find(l + 1);
                        if (it == levels.end())
                            return 0;

                        auto edgeIdx = [](int dir, int j) {
                            switch (dir)
                            {
                            case 0: return j * 5;
                            case 1: return j;
                            case 2: return 4 + j * 5;
                            case 3:
                            default:
                                return 4 * 5 + j;
                            }
                            assert(false);
                        };

                        int count{};
                        for (int j = 0; j < 5; ++j)
                            count += it->second.count(edgeIdx(dir, j));
                        return count;
                    }

                    auto countOuter = [&](int idx) -> int {
                        auto it = levels.find(l - 1);
                        if (it == levels.end())
                            return 0;
                        return it->second.count(idx);
                    };

                    if (r < 0)
                        return countOuter(getIdx(1, 2));
                    if (r > 4)
                        return countOuter(getIdx(3, 2));
                    if (c < 0)
                        return countOuter(getIdx(2, 1));
                    if (c > 4)
                        return countOuter(getIdx(2, 3));

                    auto it = levels.find(l);
                    if (it == levels.end())
                        return 0;
                    return it->second.count(getIdx(r, c));
                };

                auto getCount = [&](int l, int r, int c) {
                    return getCountImpl(l, r, c + 1, 0) + getCountImpl(l, r + 1, c, 1) + getCountImpl(l, r, c - 1, 2) + getCountImpl(l, r - 1, c, 3);
                };

                auto exists = [&](int l, int r, int c) {
                    auto it = levels.find(l);
                    if (it == levels.end())
                        return false;
                    return it->second.count(getIdx(r, c)) > 0;
                };

                LevelsT ret;

                int minLevel = levels.begin()->first;
                int maxLevel = levels.rbegin()->first;
                for (int l = minLevel - 1, ln = maxLevel + 1; l <= ln; ++l)
                {
                    for (int r = 0; r < 5; ++r)
                        for (int c = 0; c < 5; ++c)
                        {
                            if (r == 2 && c == 2)
                                continue;
                            auto cnt = getCount(l, r, c);
                            if (exists(l, r, c))
                            {
                                if (cnt == 1)
                                    ret[l].insert(getIdx(r, c));
                            }
                            else
                            {
                                if (cnt == 1 || cnt == 2)
                                    ret[l].insert(getIdx(r, c));
                            }
                        }
                }

                return ret;
            };

            auto countBugs = [&](const LevelsT &levels) {
                return std::accumulate(begin(levels), end(levels), 0, [](int s, const auto &l) { return s + l.second.size(); });
            };

            //auto dump = [&](const LevelsT &levels) {
            //    for (auto &[i, l] : levels)
            //    {
            //        std::cout << "Level " << i << "\n";
            //        for (int r = 0; r < 5; ++r)
            //        {
            //            for (int c = 0; c < 5; ++c)
            //            {
            //                std::cout << (l.count(getIdx(r, c)) ? '#' : '.');
            //            }
            //            std::cout << "\n";
            //        }
            //    }
            //    std::cout << std::endl;
            //};

            {
                // ....#
                // #..#.
                // #.?##
                // ..#..
                // #....

                LevelsT l{{0, {4, 5, 8, 10, 13, 14, 17, 20}}};

                for (int i = 0; i < 10; ++i)
                    l = evolve(l);
                expect(99_i == countBugs(l));
            }

            {
                // ..#.#
                // .#.##
                // ##...
                // .#.#.
                // ###..

                LevelsT l{{0, {2, 4, 6, 8, 9, 10, 11, 16, 18, 20, 21, 22}}};
                for (int i = 0; i < 200; ++i)
                    l = evolve(l);
                Printer::Print(__FILE__, "2", countBugs(l));
            }
        }
    };
};

} //namespace;
