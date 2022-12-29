#include "../test.hpp"
#include <fstream>

namespace {

struct Snafu
{
    int64_t val{};

    Snafu(int64_t val): val{val} { }

    Snafu(std::string_view s)
    {
        for (char d : s)
        {
            val *= 5;
            switch (d)
            {
            case '2': val += 2; break;
            case '1': val += 1; break;
            case '0': break;
            case '-': val -= 1; break;
            case '=': val -= 2; break;
            }
        }
    }

    std::string Str() const
    {
        int64_t pow{1};
        for (int i = 0; i < 20; ++i)
            pow *= 5;

        auto a = val;

        auto findClosest = [&]() {
            int64_t min_dist = std::numeric_limits<int64_t>::max();
            int digit{};
            for (int d : {-2, -1, 0, 1, 2})
            {
                int64_t dist = std::abs(a - d * pow);
                if (min_dist > dist)
                {
                    min_dist = dist;
                    digit = d;
                }
            }
            return digit;
        };

        std::string s;
        while (pow)
        {
            int digit = findClosest();
            if (digit || !s.empty())
            {
                switch (digit)
                {
                case 2: s.push_back('2'); break;
                case 1: s.push_back('1'); break;
                case 0: s.push_back('0'); break;
                case -1: s.push_back('-'); break;
                case -2: s.push_back('='); break;
                }
            }
            a -= pow * digit;
            pow /= 5;
        }
        return s;
    }
};

std::string Task1(std::istream &&is)
{
    Snafu res{0};
    std::string line;
    while (std::getline(is, line))
    {
        res.val += Snafu{line}.val;
    }
    return res.Str();
}

const char *const TEST =
    "1=-0-2\n"
    "12111\n"
    "2=0=\n"
    "21\n"
    "2=01\n"
    "111\n"
    "20012\n"
    "112\n"
    "1=-1=\n"
    "1-12\n"
    "12\n"
    "1=\n"
    "122";

using namespace boost::ut;

suite s = [] {
    "2022-25"_test = [] {
        expect(Snafu("1=-0-2").val == 1747_i);
        expect(Snafu("12111").val == 906_i);
        expect(Snafu("2=0=").val == 198_i);
        expect(Snafu("21").val == 11_i);
        expect(Snafu("2=01").val == 201_i);
        expect(Snafu("111").val == 31_i);
        expect(Snafu("20012").val == 1257_i);
        expect(Snafu("112").val == 32_i);
        expect(Snafu("1=-1=").val == 353_i);
        expect(Snafu("1-12").val == 107_i);
        expect(Snafu("12").val == 7_i);
        expect(Snafu("1=").val == 3_i);
        expect(Snafu("122").val == 37_i);

        expect(Snafu(1).Str() == "1");
        expect(Snafu(2).Str() == "2");
        expect(Snafu(3).Str() == "1=");
        expect(Snafu(4).Str() == "1-");
        expect(Snafu(5).Str() == "10");
        expect(Snafu(6).Str() == "11");
        expect(Snafu(7).Str() == "12");
        expect(Snafu(8).Str() == "2=");
        expect(Snafu(9).Str() == "2-");
        expect(Snafu(10).Str() == "20");
        expect(Snafu(15).Str() == "1=0");
        expect(Snafu(20).Str() == "1-0");
        expect(Snafu(2022).Str() == "1=11-2");
        expect(Snafu(12345).Str() == "1-0---0");
        expect(Snafu(314159265).Str() == "1121-1110-1=0");

        expect(Task1(std::istringstream{TEST}) == "2=-1=0");

        Printer::Print(__FILE__, "1", Task1(std::ifstream{INPUT}));
    };
};

} //namespace;
