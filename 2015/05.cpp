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

bool IsNice2(const std::string &s)
{
    if (s.size() < 3)
        return false;

    bool is_nice = false;

    for (size_t i = 0, n = s.size() - 2; i < n; ++i)
    {
        std::string_view p(s.data() + i, 2);
        std::string_view tail(s.data() + i + 2);
        if (tail.find(p) != tail.npos)
        {
            is_nice = true;
            break;
        }
    }

    if (!is_nice)
    {
        return false;
    }

    is_nice = false;

    for (size_t i = 0, n = s.size() - 2; i < n; ++i)
    {
        if (s[i] == s[i+2])
        {
            is_nice = true;
            break;
        }
    }

    return is_nice;
}

template <typename Pred>
size_t CountNice(std::istream &&is, Pred pred)
{
    size_t count{};

    std::string s;
    while (is && (is >> s))
    {
        if (pred(s))
        {
            ++count;
        }
    }

    return count;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test1") {
        REQUIRE(IsNice("ugknbfddgicrmopn"));
        REQUIRE(IsNice("aaa"));
        REQUIRE(!IsNice("jchzalrnumimnmhp"));
        REQUIRE(!IsNice("haegwjzuvuyypxyu"));
        REQUIRE(!IsNice("dvszwmarrgswjxmb"));
    }

    SUBCASE("test2") {
        REQUIRE(IsNice2("qjhvhtzxzqqjkmpb"));
        REQUIRE(IsNice2("xxyxx"));
        REQUIRE(!IsNice2("uurcxstgmygtbstg"));
        REQUIRE(!IsNice2("ieodomkazucvgmuy"));
    }

    SUBCASE("task") {
        MESSAGE(CountNice(std::ifstream{INPUT}, IsNice));
        MESSAGE(CountNice(std::ifstream{INPUT}, IsNice2));
    }
}
