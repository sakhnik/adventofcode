#include "../test.hpp"
#include <boost/multiprecision/gmp.hpp>

namespace {

using namespace boost::ut;

using IntT = boost::multiprecision::mpz_int;

IntT Det(IntT a1, IntT b1, IntT a2, IntT b2)
{
    return a1 * b2 - a2 * b1;
}

IntT Solve(IntT a1, IntT b1, IntT a2, IntT b2, IntT c1, IntT c2)
{
    auto d = Det(a1, b1, a2, b2);
    if (d == 0)
        return 0;
    auto da = Det(c1, b1, c2, b2);
    auto db = Det(a1, c1, a2, c2);

    if (da % d || db % d) {
        return 0;
    }
    auto a = da / d;
    auto b = db / d;
    return a * 3 + b;
}

std::pair<int, IntT> Task(std::istream &&is)
{
    const IntT SHIFT{"10000000000000"};

    int task1{};
    IntT task2{};

    while (is) {
        std::string line;
        getline(is, line);
        int dx_a{}, dy_a{};
        sscanf(line.c_str(), "Button A: X+%d, Y+%d", &dx_a, &dy_a);
        getline(is, line);
        int dx_b{}, dy_b{};
        sscanf(line.c_str(), "Button B: X+%d, Y+%d", &dx_b, &dy_b);
        getline(is, line);
        int X{}, Y{};
        sscanf(line.c_str(), "Prize: X=%d, Y=%d", &X, &Y);
        getline(is, line);

        task1 += Solve(dx_a, dx_b, dy_a, dy_b, X, Y).convert_to<int>();
        task2 += Solve(dx_a, dx_b, dy_a, dy_b, X + SHIFT, Y + SHIFT);
    }

    return {task1, task2};
}

suite s = [] {
    "13"_test = [] {
        const char *const TEST1 = R"(Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400

Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176

Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450

Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279)";
        auto test = Task(std::istringstream{TEST1});
        expect(480_i == test.first);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second.convert_to<std::string>());
    };
};

} //namespace;
