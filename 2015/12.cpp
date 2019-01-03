#include <doctest/doctest.h>
#include <fstream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace {

namespace pt = boost::property_tree;

bool NoRed(const pt::ptree &node)
{
    for (auto it : node)
    {
        if (it.first != "" && it.second.get<std::string>("") == "red")
            return false;
    }
    return true;
}

bool Anything(const pt::ptree &node)
{
    return true;
}

template <typename PredT>
int SumNums(const pt::ptree &node, PredT pred)
{
    if (!pred(node))
    {
        return 0;
    }

    int res = node.get_value(0);
    for (auto it : node)
    {
        res += SumNums(it.second, pred);
    }
    return res;
}

template <typename PredT>
int SumNums(std::istream &&is, PredT pred)
{
    pt::ptree root;
    pt::read_json(is, root);
    return SumNums(root, pred);
}

int SumNums(const char *json)
{
    return SumNums(std::istringstream{json}, Anything);
}

int SumNums2(const char *json)
{
    return SumNums(std::istringstream{json}, NoRed);
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

        REQUIRE(6 == SumNums2("[1,2,3]"));
        REQUIRE(4 == SumNums2(R"([1,{"c":"red","b":2},3])"));
        REQUIRE(0 == SumNums2(R"({"d":"red","e":[1,2,3,4],"f":5})"));
        REQUIRE(6 == SumNums2(R"([1,"red",5])"));
    }

    SUBCASE("task") {
        MESSAGE(SumNums(std::ifstream{INPUT}, Anything));
        MESSAGE(SumNums(std::ifstream{INPUT}, NoRed));
    }
}
