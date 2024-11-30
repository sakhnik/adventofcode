#include <fstream>
#include <algorithm>
#include <regex>
#include "../test.hpp"
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

using namespace boost::ut;

suite s = [] {
    "05"_test = [] {
        expect(IsNice("ugknbfddgicrmopn"));
        expect(IsNice("aaa"));
        expect(!IsNice("jchzalrnumimnmhp"));
        expect(!IsNice("haegwjzuvuyypxyu"));
        expect(!IsNice("dvszwmarrgswjxmb"));

        expect(IsNice2("qjhvhtzxzqqjkmpb"));
        expect(IsNice2("xxyxx"));
        expect(!IsNice2("uurcxstgmygtbstg"));
        expect(!IsNice2("ieodomkazucvgmuy"));

        Printer::Print(__FILE__, "1", CountNice(std::ifstream{INPUT}, IsNice));
        Printer::Print(__FILE__, "2", CountNice(std::ifstream{INPUT}, IsNice2));
    };
};

} //namespace;
