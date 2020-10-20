#include <sstream>
#include <fstream>
#include <algorithm>
#include <boost/ut.hpp>

namespace {

size_t CountEscape(std::istream &&is)
{
    size_t count{};
    std::string line;
    while (is && getline(is, line))
    {
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '\\')
            {
                ++count;
                ++i;
                if (line[i] == 'x')
                {
                    count += 2;
                    i += 2;
                }
            }
        }
        count += 2;
    }
    return count;
}

size_t CountEscape2(std::istream &&is)
{
    size_t count{};
    std::string line;
    while (is && getline(is, line))
    {
        auto c = std::count_if(line.begin(), line.end(),
                               [](char ch) {
                                    return ch == '\\' || ch == '"';
                               });
        c += 2;
        count += c;
    }
    return count;
}

using namespace boost::ut;

suite s = [] {
    "2015-08"_test = [] {
        const char *const TEST = R"(""
"abc"
"aaa\"aaa"
"\x27")";
        expect(12_u == CountEscape(std::istringstream{TEST}));
        expect(19_u == CountEscape2(std::istringstream{TEST}));

        std::cout << "2015-08.1: " << CountEscape(std::ifstream{INPUT}) << std::endl;
        std::cout << "2015-08.2: " << CountEscape2(std::ifstream{INPUT}) << std::endl;
    };
};

} //namespace;