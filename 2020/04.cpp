#include "../test.hpp"

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
    cid = 1 << 7, // (Country ID)
};

struct Field
{
    std::string_view name;
    FieldMask mask;
} DICT[] = {
    {"byr", byr},
    {"iyr", iyr},
    {"eyr", eyr},
    {"hgt", hgt},
    {"hcl", hcl},
    {"ecl", ecl},
    {"pid", pid},
    {"cid", cid},
};

bool IsValid(int fields)
{
    const int polar = byr | iyr | eyr | hgt | hcl | ecl | pid;
    if (fields == polar)
        return true;
    const int national = polar | cid;
    if (fields == national)
        return true;
    return false;
}

int CountValid(std::istream &&is)
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

        static const std::regex e{R"(\b([a-z]+):)"};
        for (std::regex_token_iterator<std::string::iterator> rend, rit{line.begin(), line.end(), e, 1};
            rit != rend;
            ++rit)
        {
            auto name = rit->str();
            auto it = std::find_if(std::begin(DICT), std::end(DICT),
                                   [&name](const auto &d) { return d.name == name; });
            if (it != std::end(DICT))
            {
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
    "2020-04"_test = [] {
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
    };
};

} //namespace;