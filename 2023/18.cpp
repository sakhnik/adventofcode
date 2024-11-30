#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Lagoon
{
    struct AreaCalculator
    {
        int64_t area{};
        int64_t perimeter{};
        int64_t x{}, y{};

        void AddSection(char dir, int64_t count)
        {
            perimeter += count;
            switch (dir)
            {
            case 'U': y -= count; break;
            case 'D': y += count; break;
            case 'L': area -= y * count; break;
            case 'R': area += y * count; break;
            }
        }

        int64_t GetResult() const
        {
            // https://en.wikipedia.org/wiki/Pick's_theorem
            return std::abs(area) + perimeter / 2 + 1;
        }
    };

    AreaCalculator task1, task2;

    Lagoon(std::istream &&is)
    {
        char dir{};
        std::string colour;
        int count{};
        while (is >> dir >> count >> colour)
        {
            task1.AddSection(dir, count);
            int count2 = std::stoi(colour.substr(2, 5), nullptr, 16);
            const char *const DIR = "RDLU";
            task2.AddSection(DIR[colour[7] - '0'], count2);
        }
    }

    int Task1() const
    {
        return task1.GetResult();
    }

    int64_t Task2() const
    {
        return task2.GetResult();
    }
};

suite s = [] {
    "18"_test = [] {
        const char *const TEST1 = R"(R 6 (#70c710)
D 5 (#0dc571)
L 2 (#5713f0)
D 2 (#d2c081)
R 2 (#59c680)
D 2 (#411b91)
L 5 (#8ceee2)
U 2 (#caa173)
L 1 (#1b58a2)
U 2 (#caa171)
R 2 (#7807d2)
U 3 (#a77fa3)
L 2 (#015232)
U 2 (#7a21e3)
)";
        Lagoon test1{std::istringstream{TEST1}};
        expect(62_i == test1.Task1());
        expect(eq(952408144115ll, test1.Task2()));

        Lagoon lagoon{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", lagoon.Task1());
        Printer::Print(__FILE__, "2", lagoon.Task2());
    };
};

} //namespace;
