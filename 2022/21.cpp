#include "../test.hpp"
#include <fstream>

namespace {

struct Monkeys
{
    struct Expr
    {
        bool is_val = true;
        int64_t val{};
        std::string a, b;
        char op{};

        Expr(int64_t val = 0)
            : is_val{true}
            , val{val}
        {
        }

        Expr(const std::string &a, char op, const std::string &b)
            : is_val{false}
            , a{a}
            , b{b}
            , op{op}
        {
        }
    };

    using ExprT = std::unordered_map<std::string, Expr>;
    ExprT expr;

    Monkeys(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            char v[32];
            char a[32], b[32];
            char op;
            int64_t val{};
            if (4 == sscanf(line.c_str(), "%s %s %c %s", v, a, &op, b))
            {
                std::string name{v};
                name.pop_back();
                expr[name] = Expr(std::string{a}, op, std::string{b});
            }
            else if (2 == sscanf(line.c_str(), "%s %ld", v, &val))
            {
                std::string name{v};
                name.pop_back();
                expr[name] = Expr(val);
            }
        }
    }

    using EvalCtxT = std::unordered_map<std::string, int64_t>;

    int64_t Eval(const std::string &name, const ExprT &expr, EvalCtxT &ctx) const
    {
        auto it = ctx.find(name);
        if (it != ctx.end())
            return it->second;
        const auto &e = expr.at(name);
        if (e.is_val)
            return ctx[name] = e.val;
        auto a = Eval(e.a, expr, ctx);
        auto b = Eval(e.b, expr, ctx);
        auto oper = [&]() {
            switch (e.op)
            {
            case '+': return a + b; break;
            case '-': return a - b; break;
            case '*': return a * b; break;
            case '/': return a / b; break;
            default: throw std::runtime_error("Unknown operation");
            }
        };
        return ctx[name] = oper();
    }

    int64_t Task1()
    {
        EvalCtxT ctx;
        return Eval("root", expr, ctx);
    }

    int64_t Task2()
    {
        EvalCtxT ctx;
        ExprT solution;

        using PromiseT = std::pair<bool, int64_t>;

        auto eval = [&](const std::string &name, auto &&eval) -> PromiseT {
            auto it = ctx.find(name);
            if (it != ctx.end())
                return {true, it->second};

            const auto &e = expr.at(name);
            if (name == "root")
            {
                auto a = eval(e.a, eval);
                auto b = eval(e.b, eval);
                if (a.first)
                    ctx[e.b] = a.second;
                else if (b.first)
                    ctx[e.a] = b.second;
                else
                    throw std::runtime_error("Can't solve");
                return {true, 0};
            }
            if (name == "humn")
                return {false, 0};

            if (e.is_val)
                return {true, ctx[name] = e.val};

            auto a = eval(e.a, eval);
            auto b = eval(e.b, eval);
            if (!a.first && !b.first)
                throw std::runtime_error("No solution");
            if (a.first && b.first)
            {
                auto oper = [&]() {
                    switch (e.op)
                    {
                    case '+': return a.second + b.second; break;
                    case '-': return a.second - b.second; break;
                    case '*': return a.second * b.second; break;
                    case '/': return a.second / b.second; break;
                    default: throw std::runtime_error("Unknown operation");
                    }
                };
                return {true, ctx[name] = oper()};
            }
            if (!a.first)
            {
                switch (e.op)
                {
                case '+': solution[e.a] = Expr(name, '-', e.b); break;
                case '-': solution[e.a] = Expr(name, '+', e.b); break;
                case '*': solution[e.a] = Expr(name, '/', e.b); break;
                case '/': solution[e.a] = Expr(name, '*', e.b); break;
                }
            }
            if (!b.first)
            {
                switch (e.op)
                {
                case '+': solution[e.b] = Expr(name, '-', e.a); break;
                case '-': solution[e.b] = Expr(e.a, '-', name); break;
                case '*': solution[e.b] = Expr(name, '/', e.a); break;
                case '/': solution[e.b] = Expr(e.a, '/', name); break;
                }
            }
            return {false, 0};
        };
        eval("root", eval);

        return Eval("humn", solution, ctx);
    }
};

const char *const TEST = R"(root: pppw + sjmn
dbpl: 5
cczh: sllz + lgvd
zczc: 2
ptdq: humn - dvpt
dvpt: 3
lfqf: 4
humn: 5
ljgn: 2
sjmn: drzm * dbpl
sllz: 4
pppw: cczh / lfqf
lgvd: ljgn * ptdq
drzm: hmdt - zczc
hmdt: 32)";

using namespace boost::ut;

suite s = [] {
    "2022-21"_test = [] {
        Monkeys test{std::istringstream{TEST}};
        expect(152_i == test.Task1());
        expect(301_i == test.Task2());

        Monkeys monkeys{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", monkeys.Task1());
        Printer::Print(__FILE__, "2", monkeys.Task2());
    };
};

} //namespace;
