#include <doctest/doctest.h>
#include <fstream>
#include <algorithm>
#include <regex>

namespace {

bool IsNice(const std::string &s)
{
    size_t pos = 0;
    for (int i = 0; i < 3; ++i, ++pos)
    {
        pos = s.find_first_of("aeiou", pos);
        if (pos == s.npos)
        {
            return false;
        }
    }

    auto it = std::adjacent_find(s.begin(), s.end(),
                                 [](char a, char b) {
                                    return a == b;
                                 });
    if (it == s.end())
    {
        return false;
    }

    static const std::regex r(R"((ab|cd|pq|xy))");
    if (regex_search(s, r))
    {
        return false;
    }

    return true;
}

size_t CountNice(std::istream &&is)
{
    size_t count{};

    std::string s;
    while (is && (is >> s))
    {
        if (IsNice(s))
        {
            ++count;
        }
    }

    return count;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        REQUIRE(IsNice("ugknbfddgicrmopn"));
        REQUIRE(IsNice("aaa"));
        REQUIRE(!IsNice("jchzalrnumimnmhp"));
        REQUIRE(!IsNice("haegwjzuvuyypxyu"));
        REQUIRE(!IsNice("dvszwmarrgswjxmb"));
    }

    SUBCASE("task") {
        MESSAGE(CountNice(std::ifstream{INPUT}));
    }
}
