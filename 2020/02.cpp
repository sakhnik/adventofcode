#include "../test.hpp"

namespace {

using namespace boost::ut;

bool IsValid(const std::string &pass)
{
    const static std::regex r{"([0-9]+)-([0-9]+) (.): (.+)"};
    std::smatch m;
    bool ok = std::regex_match(pass, m, r);
    expect(ok);
    int min = std::stoi(m[1]);
    int max = std::stoi(m[2]);
    char c = *m[3].first;

    int count = std::count(m[4].first, m[4].second, c);
    return count >= min && count <= max;
}

int CountValid(std::istream &&is)
{
    int count{};
    std::string line;
    while (getline(is, line))
    {
        if (IsValid(line))
            ++count;
    }
    return count;
}

suite s = [] {
    "2020-02"_test = [] {
        expect(IsValid("1-3 a: abcde"));
        expect(!IsValid("1-3 b: cdefg"));
        expect(IsValid("2-9 c: ccccccccc"));

        Printer::Print(__FILE__, "1", CountValid(std::ifstream{INPUT}));
    };
};

} //namespace;