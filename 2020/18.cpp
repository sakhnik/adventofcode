#include "../test.hpp"

namespace {

int64_t EvaluateImpl(const char *&expr)
{
    char op{};
    int64_t a{};

    auto accum = [&](int64_t val) {
        switch (op)
        {
        case '+': a += val; break;
        case '-': a -= val; break;
        case '*': a *= val; break;
        default: a = val; break;
        }
        op = {};
    };

    while (char ch = *expr++)
    {
        switch (ch)
        {
        case '0'...'9':
            accum(ch - '0');
            break;
        case '+':
        case '-':
        case '*':
            op = ch;
            break;
        case '(':
            accum(EvaluateImpl(expr));
            break;
        case ')':
            return a;
        case ' ':
            continue;
        default:
            throw "Bad char";
        }
    }
    return a;
}

int64_t Evaluate(const char *expr)
{
    return EvaluateImpl(expr);
}

int64_t Task1(std::istream &&is)
{
    int64_t result{};

    std::string line;
    while (getline(is, line))
    {
        result += Evaluate(line.c_str());
    }
    return result;
}

using namespace boost::ut;

suite s = [] {
    "2020-18"_test = [] {
        expect(71_i == Evaluate("1 + 2 * 3 + 4 * 5 + 6"));
        expect(51_i == Evaluate("1 + (2 * 3) + (4 * (5 + 6))"));
        expect(26_i == Evaluate("2 * 3 + (4 * 5)"));
        expect(437_i == Evaluate("5 + (8 * 3 + 9 + 3 * 4 * 3)"));
        expect(12240_i == Evaluate("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))"));
        expect(13632_i == Evaluate("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2"));

        Printer::Print(__FILE__, "1", Task1(std::ifstream{INPUT}));
    };
};

} //namespace;
