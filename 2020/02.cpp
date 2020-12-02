#include "../test.hpp"

namespace {

using namespace boost::ut;

using ChecksT = std::pair<bool,bool>;

ChecksT IsValid(const std::string &pass)
{
    const static std::regex r{"([0-9]+)-([0-9]+) (.): (.+)"};
    std::smatch m;
    bool ok = std::regex_match(pass, m, r);
    expect(ok);
    int min = std::stoi(m[1]);
    int max = std::stoi(m[2]);
    char c = *m[3].first;

    ChecksT checks{};

    int count = std::count(m[4].first, m[4].second, c);
    checks.first = count >= min && count <= max;

    bool hasFirst = min <= m[4].length() && m[4].first[min - 1] == c;
    bool hasSecond = max <= m[4].length() && m[4].first[max - 1] == c;
    checks.second = hasFirst != hasSecond;

    return checks;
}

using CountsT = std::pair<int,int>;

CountsT CountValid(std::istream &&is)
{
    CountsT counts{};
    std::string line;
    while (getline(is, line))
    {
        auto is_valid = IsValid(line);
        counts.first += is_valid.first;
        counts.second += is_valid.second;
    }
    return counts;
}

suite s = [] {
    "2020-02"_test = [] {
        expect(IsValid("1-3 a: abcde") == ChecksT{true, true});
        expect(IsValid("1-3 b: cdefg") == ChecksT{false, false});
        expect(IsValid("2-9 c: ccccccccc") == ChecksT{true, false});

        auto counts = CountValid(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", counts.first);
        Printer::Print(__FILE__, "2", counts.second);
    };
};

} //namespace;