#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

unsigned Parse(const std::string &id)
{
    unsigned code{};
    for (size_t i = 0; i < id.size(); ++i)
    {
        code <<= 1;
        const std::string_view upper{"BR"};
        if (upper.find(id[i]) != upper.npos)
        {
            code |= 1;
        }
    }
    return code;
}

unsigned FindHighest(std::istream &&is)
{
    unsigned highest_code{};
    std::string id;
    while (getline(is, id))
    {
        unsigned code = Parse(id);
        if (highest_code < code)
            highest_code = code;
    }
    return highest_code;
}

suite s = [] {
    "2020-05"_test = [] {
        expect(357_u == Parse("FBFBBFFRLR"));
        expect(567_u == Parse("BFFFBBFRRR"));
        expect(119_u == Parse("FFFBBBFRRR"));
        expect(820_u == Parse("BBFFBBFRLL"));

        Printer::Print(__FILE__, "1", FindHighest(std::ifstream{INPUT}));
    };
};

} //namespace;
