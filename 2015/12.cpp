#include <doctest/doctest.h>
#include <fstream>
#include <regex>

namespace {

int SumNums(const std::string &json)
{
    int res{};
    static const std::regex r(R"(-?\d+)");
    for (std::regex_iterator<std::string::const_iterator>
            rit(json.begin(), json.end(), r),
            rend;
        rit != rend;
        ++rit)
    {
        res += std::stoi(rit->str());
    }
    return res;
}

int SumNums(std::istream &&is)
{
    std::string line;
    getline(is, line);
    return SumNums(line);
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        REQUIRE(6 == SumNums("[1,2,3]"));
        REQUIRE(6 == SumNums(R"({"a":2,"b":4})"));
        REQUIRE(3 == SumNums(R"([[[3]]])"));
        REQUIRE(3 == SumNums(R"({"a":{"b":4},"c":-1})"));
        REQUIRE(0 == SumNums(R"({"a":[-1,1]})"));
        REQUIRE(0 == SumNums(R"([-1,{"a":1}])"));
        REQUIRE(0 == SumNums("[]"));
        REQUIRE(0 == SumNums("{}"));
    }

    SUBCASE("task") {
        MESSAGE(SumNums(std::ifstream{INPUT}));
    }
}
