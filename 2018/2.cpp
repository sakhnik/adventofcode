#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

using BoxesT = std::vector<std::string>;

BoxesT GetInput()
{
    BoxesT ret;
    std::ifstream ifs(INPUT);
    std::string line;
    while (std::getline(ifs, line))
    {
        ret.emplace_back(std::move(line));
    }
    return ret;
}

int Checksum(const BoxesT &boxes)
{
    int twos{}, threes{};

    for (auto &box : boxes)
    {
        std::unordered_map<char, int> counts;
        for (char c : box)
        {
            ++counts[c];
        }

        auto hasCount = [&counts](int cnt) {
            return std::find_if(begin(counts), end(counts),
                                [cnt](auto c) { return c.second == cnt; })
                != end(counts);
        };

        if (hasCount(2))
        {
            ++twos;
        }

        if (hasCount(3))
        {
            ++threes;
        }
    }

    return twos * threes;
}

TEST_CASE("main")
{
    auto test_cs = Checksum({"abcdef", "bababc", "abbcde", "abcccd", "aabcdd", "abcdee", "ababab"});
    REQUIRE(12 == test_cs);

    std::cout << Checksum(GetInput()) << std::endl;
}
