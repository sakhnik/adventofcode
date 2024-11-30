#include "../test.hpp"
#include <fstream>
#include <boost/json.hpp>

namespace {

std::strong_ordering operator<=>(const boost::json::value &a, const boost::json::value &b)
{
    if (a.is_number() && b.is_number())
        return a.as_int64() <=> b.as_int64();
    if (a.is_number())
        return boost::json::array(1, a.as_int64()) <=> b;
    if (b.is_number())
        return a <=> boost::json::array(1, b.as_int64());

    const auto &al = a.as_array();
    const auto &bl = b.as_array();

    auto common = std::min(al.size(), bl.size());
    for (int i = 0; i < common; ++i)
    {
        auto r = al[i] <=> bl[i];
        if (r != r.equal)
            return r;
    }
    return al.size() <=> bl.size();
}

int Task1(std::istream &&is)
{
    int res{};
    int idx{1};
    std::string l1, l2;
    while (std::getline(is, l1))
    {
        if (l1.empty())
            std::getline(is, l1);
        std::getline(is, l2);
        auto a = boost::json::parse(l1);
        auto b = boost::json::parse(l2);
        if (a < b)
            res += idx;
        ++idx;
    }
    return res;
}

int Task2(std::istream &&is)
{
    std::vector<boost::json::value> msgs;
    const auto m1 = boost::json::parse("[[2]]");
    const auto m2 = boost::json::parse("[[6]]");
    msgs.push_back(m1);
    msgs.push_back(m2);

    std::string line;
    while (std::getline(is, line))
    {
        if (line.empty())
            continue;
        msgs.emplace_back(boost::json::parse(line));
    }

    std::vector<size_t> idx(msgs.size(), 0);
    std::iota(begin(idx), end(idx), 0);

    std::sort(begin(idx), end(idx), [&](size_t a, size_t b) {
        return msgs[a] < msgs[b];
    });

    auto i1 = std::find(begin(idx), end(idx), 0);
    auto i2 = std::find(begin(idx), end(idx), 1);
    return (1 + i1 - begin(idx)) * (1 + i2 - begin(idx));
}

const char *const TEST = R"([1,1,3,1,1]
[1,1,5,1,1]

[[1],[2,3,4]]
[[1],4]

[9]
[[8,7,6]]

[[4,4],4,4]
[[4,4],4,4,4]

[7,7,7,7]
[7,7,7]

[]
[3]

[[[]]]
[[]]

[1,[2,[3,[4,[5,6,7]]]],8,9]
[1,[2,[3,[4,[5,6,0]]]],8,9])";

using namespace boost::ut;

suite s = [] {
    "13"_test = [] {
        expect(13_i == Task1(std::istringstream{TEST}));
        expect(140_i == Task2(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", Task1(std::ifstream{INPUT}));
        Printer::Print(__FILE__, "2", Task2(std::ifstream{INPUT}));
    };
};

} //namespace;
