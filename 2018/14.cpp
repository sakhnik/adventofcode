#include <doctest/doctest.h>
#include <string>

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
        auto sum = std::to_string(r1 + r2);
        _seq += sum;
        _pos1 = (_pos1 + r1 + 1) % _seq.size();
        _pos2 = (_pos2 + r2 + 1) % _seq.size();
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

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test1") {
        Recipes r;
        REQUIRE("37" == r.GetSeq());
        REQUIRE("3710" == r.Step());
        REQUIRE("371010" == r.Step());
        REQUIRE("3710101" == r.Step());
    }

    SUBCASE("test2") {
        REQUIRE("5158916779" == Recipes{}.Get10After(9));
        REQUIRE("0124515891" == Recipes{}.Get10After(5));
        REQUIRE("9251071085" == Recipes{}.Get10After(18));
        REQUIRE("5941429882" == Recipes{}.Get10After(2018));
    }

    SUBCASE("task1") {
        MESSAGE(Recipes{}.Get10After(323081));
    }

    SUBCASE("test3") {
        REQUIRE(9 == Recipes{}.RunUntil("51589"));
        REQUIRE(5 == Recipes{}.RunUntil("01245"));
        REQUIRE(18 == Recipes{}.RunUntil("92510"));
        REQUIRE(2018 == Recipes{}.RunUntil("59414"));
    }

    SUBCASE("task2") {
        MESSAGE(Recipes{}.RunUntil("323081"));
    }
}
