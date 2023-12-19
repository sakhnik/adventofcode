#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Workflows
{
    struct Part
    {
        std::unordered_map<char, int> params;

        int GetSum() const
        {
            return std::accumulate(params.begin(), params.end(), 0, [](int a, const auto &p) { return a + p.second; });
        }
    };

    using EvalT = std::function<bool(const Part &p)>;
    std::unordered_map<std::string, EvalT> rules;

    int task1{};

    Workflows(std::istream &&is)
    {
        rules["A"] = [](const Part &p) { return true; };
        rules["R"] = [](const Part &p) { return false; };

        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
                break;

            struct Condition
            {
                char field{};
                char op{};
                int val{};
                std::string next;
            };
            std::vector<Condition> conditions;

            auto idx = line.find('{');
            std::string name = line.substr(0, idx);
            line.erase(0, idx + 1);
            line.erase(line.size() - 1, 1);
            std::istringstream iss{line};
            std::string rule;
            while (getline(iss, rule, ','))
            {
                static const std::regex opr(R"((.)([<>])(\d+):(.+))");
                std::string srule{rule.begin(), rule.end()};
                std::smatch m;
                if (std::regex_match(srule, m, opr))
                {
                    Condition condition;
                    condition.field = *m[1].first;
                    condition.op = *m[2].first;
                    condition.val = std::stoi(m[3]);
                    condition.next = m[4];
                    conditions.push_back(condition);
                }
                else
                {
                    Condition condition;
                    condition.next = rule;
                    conditions.push_back(condition);
                }
            }

            rules[name] = [=, this](const Part &p) {
                for (auto &condition : conditions)
                {
                    switch (condition.op)
                    {
                    case 0:
                        return rules.at(condition.next)(p);
                    case '<':
                        if (p.params.at(condition.field) < condition.val)
                            return rules.at(condition.next)(p);
                        break;
                    case '>':
                        if (p.params.at(condition.field) > condition.val)
                            return rules.at(condition.next)(p);
                        break;
                    }
                }
                assert(false);
                return false;
            };
        }

        while (getline(is, line))
        {
            line = line.substr(1, line.size() - 2);
            std::istringstream iss{line};
            Part p;
            std::string def;
            while (getline(iss, def, ','))
            {
                char field = def[0];
                int val = std::stoi(std::string{def.substr(2)});
                p.params[field] = val;
            }
            if (rules.at("in")(p))
                task1 += p.GetSum();
        }
    }

    int Task1() const { return task1; }
};

suite s = [] {
    "2023-19"_test = [] {
        const char *const TEST1 = R"(px{a<2006:qkq,m>2090:A,rfg}
pv{a>1716:R,A}
lnx{m>1548:A,A}
rfg{s<537:gd,x>2440:R,A}
qs{s>3448:A,lnx}
qkq{x<1416:A,crn}
crn{x>2662:A,R}
in{s<1351:px,qqz}
qqz{s>2770:qs,m<1801:hdj,R}
gd{a>3333:R,R}
hdj{m>838:A,pv}

{x=787,m=2655,a=1222,s=2876}
{x=1679,m=44,a=2067,s=496}
{x=2036,m=264,a=79,s=2244}
{x=2461,m=1339,a=466,s=291}
{x=2127,m=1623,a=2188,s=1013}
)";
        Workflows test1{std::istringstream{TEST1}};
        expect(19114_i == test1.Task1());
        //expect(eq(952408144115ll, test1.Task2()));

        Workflows task{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", task.Task1());
        //Printer::Print(__FILE__, "2", task.Task2());
    };
};

} //namespace;
