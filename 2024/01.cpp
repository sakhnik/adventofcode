#include "../test.hpp"

namespace {

int Distance(std::istream &&is)
{
    std::vector<int> left, right;
    int a{}, b{};
    while (is >> a >> b) {
        left.push_back(a);
        right.push_back(b);
    }
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    int r{};
    for (int i = 0; i < left.size(); ++i) {
        r += std::abs(right[i] - left[i]);
    }
    return r;
}

int SimilarityScore(std::istream &&is)
{
    std::vector<int> left;
    std::unordered_map<int, int> right;
    int a{}, b{};
    while (is >> a >> b) {
        left.push_back(a);
        ++right[b];
    }

    int r{};
    for (auto i : left) {
        r += i * right[i];
    }
    return r;
}

using namespace boost::ut;

suite s = [] {
    "01"_test = [] {
        const char *const TEST1 = R"(3   4
4   3
2   5
1   3
3   9
3   3
)";
        expect(11_i == Distance(std::istringstream{TEST1}));
        expect(31_i == SimilarityScore(std::istringstream{TEST1}));

        Printer::Print(__FILE__, "1", Distance(std::ifstream{INPUT}));
        Printer::Print(__FILE__, "2", SimilarityScore(std::ifstream{INPUT}));
    };
};

} //namespace;
