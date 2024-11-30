#include <string>
#include "../test.hpp"

namespace {

class Recipes
{
public:

    const std::string &GetSeq() const
    {
        return _seq;
    }

    const std::string& Step()
    {
        auto r1 = _seq[_pos1] - '0';
        auto r2 = _seq[_pos2] - '0';
        auto sum = r1 + r2;
        if (sum > 9)
        {
            _seq.push_back('1');
            sum -= 10;
        }
        _seq.push_back('0' + sum);
        _pos1 += r1 + 1;
        while (_pos1 >= _seq.size())
            _pos1 -= _seq.size();
        _pos2 += r2 + 1;
        while (_pos2 >= _seq.size())
            _pos2 -= _seq.size();
        return _seq;
    }

    std::string_view Get10After(size_t count)
    {
        while (_seq.size() < count + 10)
        {
            Step();
        }

        return std::string_view(_seq.data() + count, 10);
    }

    size_t RunUntil(std::string_view comb)
    {
        while (true)
        {
            Step();
            if (_seq.size() < comb.size())
                continue;
            auto start = _seq.size() - comb.size();
            auto len = comb.size();
            if (start > 0)
            {
                // Every step may increment the sequence up to two digits.
                --start;
                ++len;
            }

            auto idx = std::string_view{_seq.data() + start, len}.find(comb);
            if (idx != std::string_view::npos)
            {
                return start + idx;
            }
        }
    }

private:
    std::string _seq{"37"};
    size_t _pos1{0};
    size_t _pos2{1};
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "14"_test = [] {
        {
            Recipes r;
            expect(eq("37"s, r.GetSeq()));
            expect(eq("3710"s, r.Step()));
            expect(eq("371010"s, r.Step()));
            expect(eq("3710101"s, r.Step()));
        }

        expect(eq("5158916779"s, Recipes{}.Get10After(9)));
        expect(eq("0124515891"s, Recipes{}.Get10After(5)));
        expect(eq("9251071085"s, Recipes{}.Get10After(18)));
        expect(eq("5941429882"s, Recipes{}.Get10After(2018)));

        Printer::Print(__FILE__, "1", Recipes{}.Get10After(323081));

        expect(9_u == Recipes{}.RunUntil("51589"));
        expect(5_u == Recipes{}.RunUntil("01245"));
        expect(18_u == Recipes{}.RunUntil("92510"));
        expect(2018_u == Recipes{}.RunUntil("59414"));

        Printer::Print(__FILE__, "2", Recipes{}.RunUntil("323081"));
    };
};

} //namespace;
