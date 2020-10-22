#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include "../test.hpp"
namespace {

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

std::string DiffId(const std::string &a, const std::string &b)
{
    std::string d;
    for (size_t i = 0; i < std::size(a); ++i)
    {
        if (a[i] == b[i])
        {
            d.push_back(a[i]);
        }
    }
    return d;
}

std::string FindCorrect(const BoxesT &boxes)
{
    for (size_t i = 0, in = boxes.size() - 1; i < in; ++i)
    {
        for (size_t j = i + 1; j < boxes.size(); ++j)
        {
            auto diff = DiffId(boxes[i], boxes[j]);
            if (size(diff) == size(boxes[i]) - 1)
            {
                return diff;
            }
        }
    }
    return "";
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2018-02"_test = [] {
        auto test_cs = Checksum({"abcdef", "bababc", "abbcde", "abcccd", "aabcdd", "abcdee", "ababab"});
        expect(12_i == test_cs);

        Printer::Print(__FILE__, "1", Checksum(GetInput()));

        auto test2 = FindCorrect({"abcde", "fghij", "klmno", "pqrst", "fguij", "axcye", "wvxyz"});
        expect(eq("fgij"s, test2));

        Printer::Print(__FILE__, "2", FindCorrect(GetInput()));
    };
};

} //namespace;