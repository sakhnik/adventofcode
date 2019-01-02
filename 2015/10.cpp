#include <doctest/doctest.h>
#include <string>

namespace {

void Rle(const std::string &src, std::string &dst)
{
    dst.clear();
    size_t start = 0;
    for (size_t i = 1; i < src.size(); ++i)
    {
        if (src[i] != src[start])
        {
            auto count = i - start;
            if (count < 10)
            {
                dst.push_back(count + '0');
            }
            else
            {
                dst += std::to_string(i - start);
            }
            dst.push_back(src[start]);
            start = i;
        }
    }
    dst += std::to_string(src.size() - start);
    dst.push_back(src[start]);
}

std::string Rle(const std::string &src)
{
    std::string dst;
    Rle(src, dst);
    return dst;
}

std::string Run(int count, std::string s)
{
    std::string buf;
    while (count--)
    {
        Rle(s, buf);
        s.swap(buf);
    }
    return s;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        REQUIRE("11" == Rle("1"));
        REQUIRE("21" == Rle("11"));
        REQUIRE("1211" == Rle("21"));
        REQUIRE("111221" == Rle("1211"));
    }

    SUBCASE("task") {
        MESSAGE(Run(40, "3113322113").size());
        MESSAGE(Run(50, "3113322113").size());
    }
}
