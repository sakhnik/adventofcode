#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

const char *const DIGITS[] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

int GetDigit(std::string_view s)
{
    if (isdigit(s[0]))
        return s[0] - '0';
    auto it = std::find_if(std::begin(DIGITS), std::end(DIGITS), [s](std::string_view d) { return s.starts_with(d); });
    return it - std::begin(DIGITS);
}

int Calibrate1(std::istream &&is)
{
    int sum{};

    std::string line;
    while (getline(is, line))
    {
        sum += 10 * (*std::find_if(line.begin(), line.end(), isdigit) - '0');
        sum += *std::find_if(line.rbegin(), line.rend(), isdigit) - '0';
    }

    return sum;
}

int Calibrate2(std::istream &&is)
{
    int sum{};

    std::string line;
    while (getline(is, line))
    {
        int a{}, b{};
        for (int i = 0; i < line.size(); ++i)
        {
            std::string_view head{line.c_str() + i};
            if ((a = GetDigit(head)) < 10)
                break;
        }

        for (int i = line.size() - 1; i >= 0; --i)
        {
            std::string_view tail{line.c_str() + i};
            if ((b = GetDigit(tail)) < 10)
                break;
        }

        assert(a <= 9 && b <= 9);
        sum += 10 * a + b;
    }

    return sum;
}

suite s = [] {
    "01"_test = [] {
        const char *const TEST1 = R"(1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet
)";
        expect(142_i == Calibrate1(std::istringstream{TEST1}));

        const char *const TEST2 = R"(two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen
)";
        expect(281_i == Calibrate2(std::istringstream{TEST2}));

        Printer::Print(__FILE__, "1", Calibrate1(std::ifstream{INPUT}));
        Printer::Print(__FILE__, "2", Calibrate2(std::ifstream{INPUT}));
    };
};

} //namespace;
