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

using ExprT = std::function<int64_t(void)>;

ExprT ParseImpl(const char *&str)
{
    std::stack<ExprT> args;
    bool add_pending = false;

    auto push = [&](ExprT expr) {
        if (add_pending)
        {
            ExprT a = args.top();
            args.pop();
            args.push([a,expr] { return a() + expr(); });
            add_pending = false;
        }
        else
            args.push(expr);
    };

    auto eval_mul = [&] {
        ExprT a = args.top();
        while (args.size() > 1)
        {
            args.pop();
            ExprT b = args.top();
            a = [a, b] { return a() * b(); };
        }
        return a;
    };

    while (char ch = *str++)
    {
        switch (ch)
        {
        case '0'...'9':
            {
                int64_t a = ch - '0';
                push([a] { return a; });
            }
            break;
        case '+':
            add_pending = true;
            break;
        case '*':
            break;
        case '(':
            push(ParseImpl(str));
            break;
        case ')':
            return eval_mul();
        case ' ':
            continue;
        default:
            throw "Bad char";
        }
    }
    return eval_mul();
}

int64_t Evaluate2(const char *str)
{
    auto expr = ParseImpl(str);
    return expr();
}

template <typename EvalT>
int64_t Task(EvalT eval, std::istream &&is)
{
    int64_t result{};

    std::string line;
    while (getline(is, line))
    {
        result += eval(line.c_str());
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

        Printer::Print(__FILE__, "1", Task(Evaluate, std::ifstream{INPUT}));

        expect(231_i == Evaluate2("1 + 2 * 3 + 4 * 5 + 6 "));
        expect(51_i == Evaluate2("1 + (2 * 3) + (4 * (5 + 6))"));
        expect(46_i == Evaluate2("2 * 3 + (4 * 5)"));
        expect(1445_i == Evaluate2("5 + (8 * 3 + 9 + 3 * 4 * 3)"));
        expect(669060_i == Evaluate2("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))"));
        expect(23340_i == Evaluate2("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2"));

        Printer::Print(__FILE__, "2", Task(Evaluate2, std::ifstream{INPUT}));
    };
};

} //namespace;
