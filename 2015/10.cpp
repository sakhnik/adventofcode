#include <string>
#include "../test.hpp"
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

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2015-10"_test = [] {
        expect(eq("11"s, Rle("1")));
        expect(eq("21"s, Rle("11")));
        expect(eq("1211"s, Rle("21")));
        expect(eq("111221"s, Rle("1211")));

        Printer::Print(__FILE__, "1", Run(40, "3113322113").size());
        Printer::Print(__FILE__, "2", Run(50, "3113322113").size());
    };
};

} //namespace;