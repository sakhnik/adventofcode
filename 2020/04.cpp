#include "../test.hpp"
#include <regex>
#include <fstream>
#include <sstream>

namespace {

enum FieldMask
{
    byr = 1 << 0, // (Birth Year)
    iyr = 1 << 1, // (Issue Year)
    eyr = 1 << 2, // (Expiration Year)
    hgt = 1 << 3, // (Height)
    hcl = 1 << 4, // (Hair Color)
    ecl = 1 << 5, // (Eye Color)
    pid = 1 << 6, // (Passport ID)
//    cid = 1 << 7, // (Country ID)
};

template <int minY, int maxY>
bool IsYrValid(const std::string &value)
{
    //byr (Birth Year) - four digits; at least 1920 and at most 2002.
    //iyr (Issue Year) - four digits; at least 2010 and at most 2020.
    //eyr (Expiration Year) - four digits; at least 2020 and at most 2030.
    static const std::regex r{"\\d{4}"};
    if (!std::regex_match(value, r))
        return false;
    int v = std::stoi(value);
    return minY <= v && v <= maxY;
}

bool IsHgtValid(const std::string &value)
{
    //hgt (Height) - a number followed by either cm or in:
    //    If cm, the number must be at least 150 and at most 193.
    //    If in, the number must be at least 59 and at most 76.
    static const std::regex r{"(\\d+)(cm|in)"};
    std::smatch m;
    if (!std::regex_match(value, m, r))
        return false;
    int v = std::stoi(m[1]);
    if (m[2] == "cm")
        return 150 <= v && v <= 193;
    if (m[2] == "in")
        return 59 <= v && v <= 76;
    return false;
}

bool IsHclValid(const std::string &value)
{
    //hcl (Hair Color) - a # followed by exactly six characters 0-9 or a-f.
    static const std::regex r{"#[0-9a-f]{6}"};
    return std::regex_match(value, r);
}

bool IsEclValid(const std::string &value)
{
    //ecl (Eye Color) - exactly one of: amb blu brn gry grn hzl oth.
    return value == "amb" || value == "blu" || value == "brn" || value == "gry"
        || value == "grn" || value == "hzl" || value == "oth";
}

bool IsPidValid(const std::string &value)
{
    //pid (Passport ID) - a nine-digit number, including leading zeroes.
    static const std::regex r{"[0-9]{9}"};
    return std::regex_match(value, r);
}

    //cid (Country ID) - ignored, missing or not.

struct Field
{
    std::string_view name;
    FieldMask mask;
    std::function<bool(const std::string &)> validator;
} DICT[] = {
    {"byr", byr, IsYrValid<1920, 2002>},
    {"iyr", iyr, IsYrValid<2010, 2020>},
    {"eyr", eyr, IsYrValid<2020, 2030>},
    {"hgt", hgt, IsHgtValid},
    {"hcl", hcl, IsHclValid},
    {"ecl", ecl, IsEclValid},
    {"pid", pid, IsPidValid},
//    {"cid", cid},
};

bool IsValid(int fields)
{
    const int polar = byr | iyr | eyr | hgt | hcl | ecl | pid;
    if (fields == polar)
        return true;
//    const int national = polar | cid;
//    if (fields == national)
//        return true;
    return false;
}

int CountValid(std::istream &&is, bool checkValue = false)
{
    int count{};
    int fields{};

    std::string line;
    while (getline(is, line))
    {
        if (line.empty())
        {
            count += IsValid(fields);
            fields = 0;
            continue;
        }

        static const std::regex e{R"(\b([a-z]+):([^ ]+))"};
        for (std::regex_token_iterator<std::string::iterator> rend, rit{line.begin(), line.end(), e, {1,2}};
            rit != rend;
            ++rit)
        {
            auto name = rit->str();
            auto value = (++rit)->str();
            auto it = std::find_if(std::begin(DICT), std::end(DICT),
                                   [&name](const auto &d) { return d.name == name; });
            if (it != std::end(DICT))
            {
                if (checkValue && !it->validator(value))
                    continue;
                fields |= it->mask;
            }
        }
    }

    count += IsValid(fields);
    return count;
}

namespace bu = boost::ut;
using namespace bu;

bu::suite s = [] {
    "04"_test = [] {
        const char *const TEST =
            R"(ecl:gry pid:860033327 eyr:2020 hcl:#fffffd
byr:1937 iyr:2017 cid:147 hgt:183cm

iyr:2013 ecl:amb cid:350 eyr:2023 pid:028048884
hcl:#cfa07d byr:1929

hcl:#ae17e1 iyr:2013
eyr:2024
ecl:brn pid:760753108 byr:1931
hgt:179cm

hcl:#cfa07d eyr:2025 pid:166559648
iyr:2011 ecl:brn hgt:59in)";
        bu::expect(2_i == CountValid(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", CountValid(std::ifstream{INPUT}));

        expect(IsYrValid<1992, 2002>("2002"));
        expect(!IsYrValid<1992, 2002>("2003"));

        expect(IsHgtValid("60in"));
        expect(IsHgtValid("190cm"));
        expect(!IsHgtValid("190in"));
        expect(!IsHgtValid("190"));

        expect(IsHclValid("#123abc"));
        expect(!IsHclValid("#123abz"));
        expect(!IsHclValid("123abc"));

        expect(IsEclValid("brn"));
        expect(!IsEclValid("wat"));

        expect(IsPidValid("000000001"));
        expect(!IsPidValid("0123456789"));

        Printer::Print(__FILE__, "2", CountValid(std::ifstream{INPUT}, true));
    };
};

} //namespace;
