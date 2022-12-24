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
        using OpT = std::function<int64_t(int64_t, int64_t)>;
        OpT op;

        Expr(int64_t val = 0)
            : is_val{true}
            , val{val}
        {
        }

        Expr(const std::string &a, char op, const std::string &b)
            : is_val{false}
            , a{a}
            , b{b}
        {
            switch (op)
            {
            case '+':
                this->op = [](int64_t a, int64_t b) { return a + b; };
                break;
            case '-':
                this->op = [](int64_t a, int64_t b) { return a - b; };
                break;
            case '*':
                this->op = [](int64_t a, int64_t b) { return a * b; };
                break;
            case '/':
                this->op = [](int64_t a, int64_t b) { return a / b; };
                break;
            }
        }
    };

    std::unordered_map<std::string, Expr> expr;

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

    int64_t Task1()
    {
        std::unordered_map<std::string, int64_t> vals;
        auto eval = [&](const std::string &name, auto &&eval) -> int64_t {
            auto it = vals.find(name);
            if (it != vals.end())
                return it->second;
            const auto &e = expr.at(name);
            if (e.is_val)
                return vals[name] = e.val;
            return vals[name] = e.op(eval(e.a, eval), eval(e.b, eval));
        };
        return eval("root", eval);
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

        Monkeys monkeys{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", monkeys.Task1());
    };
};

} //namespace;
