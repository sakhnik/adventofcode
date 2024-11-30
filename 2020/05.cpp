#include "../test.hpp"
#include <fstream>
#include <set>
#include <algorithm>

namespace {

using namespace boost::ut;

class Seats
{
public:
    Seats(std::istream &&is)
    {
        std::string id;
        while (getline(is, id))
        {
            _occupied.insert(Parse(id));
        }
    }

    static unsigned Parse(const std::string &id)
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

    unsigned FindHighest() const
    {
        return *_occupied.rbegin();
    }

    unsigned FindGap() const
    {
        auto it = std::adjacent_find(_occupied.begin(), _occupied.end(),
                                     [](unsigned a, unsigned b) {
                                        return a + 1 != b;
                                     });
        return *it + 1;
    }

private:
    std::set<unsigned> _occupied;
};

suite s = [] {
    "05"_test = [] {
        expect(357_u == Seats::Parse("FBFBBFFRLR"));
        expect(567_u == Seats::Parse("BFFFBBFRRR"));
        expect(119_u == Seats::Parse("FFFBBBFRRR"));
        expect(820_u == Seats::Parse("BBFFBBFRLL"));

        Seats seats{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", seats.FindHighest());
        Printer::Print(__FILE__, "2", seats.FindGap());
    };
};

} //namespace;
