#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Bricks
{
    using DimsT = std::array<int, 3>;
    using BrickT = std::array<DimsT, 2>;

    std::vector<BrickT> bricks;
    std::unordered_map<int, std::vector<int>> bottoms, tops;

    Bricks(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            for (int i = line.find_first_of(",~"); i != line.npos; i = line.find_first_of(",~", i + 1))
                line[i] = ' ';
            std::istringstream iss{line};
            BrickT brick;
            for (int i : {0, 1})
                for (int j : {0, 1, 2})
                    iss >> brick[i][j];
            for (int j : {0, 1, 2})
                assert(brick[0][j] <= brick[1][j]);
            bricks.push_back(brick);
        }

        // Sort by Z
        std::sort(bricks.begin(), bricks.end(), [](const BrickT &a, const BrickT &b) {
            return a[0][2] < b[0][2];
        });

        int topZ = 1;
        // Insertion sort to settle down
        for (int i = 0; i < bricks.size(); ++i)
        {
            bool settled = false;
            auto brick = bricks[i];
            auto dropBy = brick[0][2] - topZ;
            if (dropBy > 0)
            {
                brick[0][2] -= dropBy;
                brick[1][2] -= dropBy;
            }
            for (int z = brick[0][2]; z > 0 && !settled; --z)
            {
                --brick[0][2];
                --brick[1][2];
                for (int j = i - 1; j >= 0; --j)
                {
                    if (AreOverlaping(bricks[j], brick))
                    {
                        settled = true;
                        break;
                    }
                }
            }

            ++brick[0][2];
            ++brick[1][2];
            bricks[i] = brick;
            topZ = std::max(topZ, brick[1][2] + 1);
            bottoms[brick[0][2]].push_back(i);
            tops[brick[1][2]].push_back(i);
        }
    }

    static bool Intersect(int a0, int a1, int b0, int b1)
    {
        assert(a0 <= a1);
        assert(b0 <= b1);
        if (a0 > b0)
            return Intersect(b0, b1, a0, a1);
        return b0 <= a1;
    }

    static bool AreOverlaping(const BrickT &a, const BrickT &b)
    {
        for (auto i : {0, 1, 2})
            if (!Intersect(a[0][i], a[1][i], b[0][i], b[1][i]))
                return false;
        return true;
    }

    int Task1()
    {
        int sum{};

        for (int i = 0; i < bricks.size(); ++i)
        {
            // Try to take out this brick
            auto brick = bricks[i];
            auto top = brick[1][2];
            // Potential dependencies
            auto it = bottoms.find(top + 1);
            if (it == bottoms.end())
            {
                // Nothing to support
                ++sum;
                continue;
            }
            // Other potential supporters
            auto it2 = tops.find(top);
            assert(it2 != tops.end());
            if (IsStableWithout(i, it2->second, it->second))
                ++sum;
        }
        return sum;
    }

    int64_t Task2()
    {
        return 0;
    }

    bool IsStableWithout(int i, const std::vector<int> &supports, const std::vector<int> &above) const
    {
        // Consider all the boxes above
        for (auto j : above)
        {
            auto higher_brick = bricks[j];
            --higher_brick[0][2];
            --higher_brick[1][2];
            bool support_found = false;
            for (auto k : supports)
            {
                if (k == i)
                    continue;  // Ignore the brick in question
                auto lower_brick = bricks[k];
                if (AreOverlaping(lower_brick, higher_brick))
                {
                    support_found = true;
                    break;
                }
            }
            if (!support_found)
                return false;
        }
        return true;
    }

    void Print() const
    {
        for (const auto &brick : bricks)
        {
            for (auto i : {0, 1})
                for (auto j : {0, 1, 2})
                    std::cerr << " " << brick[i][j];
            std::cerr << std::endl;
        }
    }
};

suite s = [] {
    "2023-22"_test = [] {
        const char *const TEST1 = R"(1,0,1~1,2,1
0,0,2~2,0,2
0,2,3~2,2,3
0,0,4~0,2,4
2,0,5~2,2,5
0,1,6~2,1,6
1,1,8~1,1,9
)";
        Bricks test1{std::istringstream{TEST1}};
        expect(5_i == test1.Task1());

        Bricks task{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", task.Task1());
        //Printer::Print(__FILE__, "2", task.Task2());
    };
};

} //namespace;
