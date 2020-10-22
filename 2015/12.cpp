#include <fstream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "../test.hpp"
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

using namespace boost::ut;

suite s = [] {
    "2015-12"_test = [] {
        expect(6_i == SumNums("[1,2,3]"));
        expect(6_i == SumNums(R"({"a":2,"b":4})"));
        expect(3_i == SumNums(R"([[[3]]])"));
        expect(3_i == SumNums(R"({"a":{"b":4},"c":-1})"));
        expect(0_i == SumNums(R"({"a":[-1,1]})"));
        expect(0_i == SumNums(R"([-1,{"a":1}])"));
        expect(0_i == SumNums("[]"));
        expect(0_i == SumNums("{}"));

        expect(6_i == SumNums2("[1,2,3]"));
        expect(4_i == SumNums2(R"([1,{"c":"red","b":2},3])"));
        expect(0_i == SumNums2(R"({"d":"red","e":[1,2,3,4],"f":5})"));
        expect(6_i == SumNums2(R"([1,"red",5])"));

        Printer::Print(__FILE__, "1", SumNums(std::ifstream{INPUT}, Anything));
        Printer::Print(__FILE__, "2", SumNums(std::ifstream{INPUT}, NoRed));
    };
};

} //namespace;