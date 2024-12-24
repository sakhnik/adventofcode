#include "../test.hpp"

namespace {

using namespace boost::ut;

using RegT = std::unordered_map<std::string, bool>;

struct Gate
{
    std::string op;
    std::string a, b;
};

using GatesT = std::unordered_map<std::string, Gate>;

bool Eval(std::string x, RegT &reg, const GatesT &gates)
{
    auto it = reg.find(x);
    if (it != reg.end())
        return it->second;
    auto it2 = gates.find(x);
    if (it2 == gates.end())
        return false;
    const Gate &g = it2->second;
    if (g.op == "AND") {
        return reg[x] = Eval(g.a, reg, gates) && Eval(g.b, reg, gates);
    } else if (g.op == "OR") {
        return reg[x] = Eval(g.a, reg, gates) || Eval(g.b, reg, gates);
    } else if (g.op == "XOR") {
        return reg[x] = Eval(g.a, reg, gates) ^ Eval(g.b, reg, gates);
    } else {
        assert(false);
    }
}

uint64_t Eval(RegT &reg, const GatesT &gates)
{
    uint64_t z{};
    for (int i = 0; i < 48; ++i) {
        char n[16];
        sprintf(n, "z%02d", i);
        if (Eval(n, reg, gates))
            z |= (1ull << i);
    }
    return z;
}

void Source(uint64_t a, const char *pref, RegT &reg)
{
    for (int i = 0; i < 48; ++i, (a >>= 1)) {
        char n[16];
        sprintf(n, "%s%02d", pref, int(a & 1));
    }
}

bool Verify(uint64_t a, uint64_t b, const GatesT &gates)
{
    RegT reg;
    Source(a, "x", reg);
    Source(b, "y", reg);
    return a + b == Eval(reg, gates);
}

std::string GetIndentImpl(int level)
{
    if (level == 0)
        return "";
    if (level == 1)
        return "  ";
    return GetIndentImpl(level / 2) + GetIndentImpl(level - level / 2);
}

std::string GetIndent(int level)
{
    return "\n" + GetIndentImpl(level);
}

struct Dissector
{
    std::unordered_map<std::string, std::string> carry;
    std::unordered_map<std::string, std::string> subst;

    std::string GetSubst(std::string a)
    {
        auto it = subst.find(a);
        return it != subst.end() ? it->second : a;
    }

    std::string GetExp(std::string root, GatesT &gates, bool s = true, int level = 0)
    {
        if (s) {
            auto a = GetSubst(root);
            if (a != root) {
                return GetExp(a, gates, false, level);
            }
        }
        if (root.starts_with('x') || root.starts_with('y'))
            return " " + GetSubst(root);
        auto it = carry.find(root);
        if (it != carry.end())
            return GetIndent(level) + it->second;
        auto it2 = gates.find(root);
        if (it2 == gates.end())
            return "";
        const auto &g = it2->second;
        auto c = GetIndent(level) + g.op + " (" + GetSubst(root) + ")";
        auto a = GetExp(g.a, gates, true, level + 1);
        auto b = GetExp(g.b, gates, true, level + 1);
        if (a.size() > b.size())
            std::swap(a, b);
        if (g.op == "OR") {
            std::string cname = "C" + std::to_string(carry.size() + 1);
            carry[root] = cname;
            return c + " -> " + cname + a + b;
        }
        return c + a + b;
    }
};

std::pair<uint64_t, std::string> Task(std::istream &&is)
{
    RegT reg;
    std::string line;
    while (getline(is, line)) {
        if (line.empty())
            break;
        std::istringstream iss{line};
        std::string s;
        uint64_t v{};
        iss >> s >> v;
        reg[s.substr(0, s.size() - 1)] = v;
    }

    GatesT gates;
    std::vector<std::string> names;

    while (getline(is, line)) {
        // x00 AND y00 -> z00
        std::istringstream iss{line};
        Gate g;
        std::string skip, c;
        iss >> g.a >> g.op >> g.b >> skip >> c;
        gates[c] = g;
        names.push_back(c);
    }

    uint64_t task1 = Eval(reg, gates);

    // Inspect the gate structure manually, follow the ripple-carry adder.
    // Fix the irregularities with the substitute table.

    Dissector d;
    d.subst["z07"] = "gmt";
    d.subst["gmt"] = "z07";
    d.subst["z18"] = "dmn";
    d.subst["dmn"] = "z18";
    d.subst["z35"] = "cfk";
    d.subst["cfk"] = "z35";
    d.subst["cbj"] = "qjj";
    d.subst["qjj"] = "cbj";

    for (int i = 0; i < 45; ++i) {
        char n[16];
        sprintf(n, "z%02d", i);
        std::cout << d.GetExp(n, gates) << std::endl;
    }
    return {task1, "cbj,cfk,dmn,gmt,qjj,z07,z18,z35"};
}

suite s = [] {
    "24"_test = [] {
        const char *const TEST1 = R"(x00: 1
x01: 1
x02: 1
y00: 0
y01: 1
y02: 0

x00 AND y00 -> z00
x01 XOR y01 -> z01
x02 OR y02 -> z02)";
        auto test = Task(std::istringstream{TEST1});
        expect(4_i == test.first);

        const char *const TEST2 = R"(x00: 1
x01: 0
x02: 1
x03: 1
x04: 0
y00: 1
y01: 1
y02: 1
y03: 1
y04: 1

ntg XOR fgs -> mjb
y02 OR x01 -> tnw
kwq OR kpj -> z05
x00 OR x03 -> fst
tgd XOR rvg -> z01
vdt OR tnw -> bfw
bfw AND frj -> z10
ffh OR nrd -> bqk
y00 AND y03 -> djm
y03 OR y00 -> psh
bqk OR frj -> z08
tnw OR fst -> frj
gnj AND tgd -> z11
bfw XOR mjb -> z00
x03 OR x00 -> vdt
gnj AND wpb -> z02
x04 AND y00 -> kjc
djm OR pbm -> qhw
nrd AND vdt -> hwm
kjc AND fst -> rvg
y04 OR y02 -> fgs
y01 AND x02 -> pbm
ntg OR kjc -> kwq
psh XOR fgs -> tgd
qhw XOR tgd -> z09
pbm OR djm -> kpj
x03 XOR y03 -> ffh
x00 XOR y04 -> ntg
bfw OR bqk -> z06
nrd XOR fgs -> wpb
frj XOR qhw -> z04
bqk OR frj -> z07
y03 OR x01 -> nrd
hwm AND bqk -> z03
tgd XOR rvg -> z12
tnw OR pbm -> gnj)";
        auto test2 = Task(std::istringstream{TEST2});
        expect(2024_i == test2.first);

        auto task = Task(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
