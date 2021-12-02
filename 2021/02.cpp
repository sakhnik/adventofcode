#include "../test.hpp"

namespace {

ssize_t Trace(std::istream &&is)
{
    ssize_t x{}, y{};

    std::string dir;
    ssize_t val{};

    while (is >> dir >> val)
    {
        if (dir == "forward")
            x += val;
        else if (dir == "down")
            y += val;
        else if (dir == "up")
            y -= val;
        else
            throw std::runtime_error("Unknown direction");
    }

    return x * y;
}

ssize_t TraceAim(std::istream &&is)
{
    ssize_t x{}, y{}, aim{};

    std::string dir;
    ssize_t val{};

    while (is >> dir >> val)
    {
        if (dir == "forward")
        {
            x += val;
            y += aim * val;
        }
        else if (dir == "down")
            aim += val;
        else if (dir == "up")
            aim -= val;
        else
            throw std::runtime_error("Unknown direction");
    }

    return x * y;
}

using namespace boost::ut;

const char *const TEST_INPUT = R"(forward 5
down 5
forward 8
up 3
down 8
forward 2)";

suite s = [] {
    "2021-02"_test = [] {
        expect(150_i == Trace(std::istringstream{TEST_INPUT}));
        Printer::Print(__FILE__, "1", Trace(std::ifstream{INPUT}));

        expect(900_i == TraceAim(std::istringstream{TEST_INPUT}));
        Printer::Print(__FILE__, "1", TraceAim(std::ifstream{INPUT}));
    };
};

} //namespace;
