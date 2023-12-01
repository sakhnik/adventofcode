#include "../test.hpp"
#include <fstream>

namespace {

const char *const TEST = R"(1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet
)";

using namespace boost::ut;

int Calibrate(std::istream &&is)
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

suite s = [] {
    "2023-01"_test = [] {
        expect(142_i == Calibrate(std::istringstream{TEST}));

        std::ifstream ifs{INPUT};
        Printer::Print(__FILE__, "1", Calibrate(std::ifstream{INPUT}));
    };
};

} //namespace;
