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
    std::vector<Part> parts;

    struct Condition
    {
        char field{};
        char op{};
        int val{};
        std::string next;
    };
    using ConditionsT = std::vector<Condition>;
    std::unordered_map<std::string, ConditionsT> rules;

    Workflows(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
                break;

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

            rules[name] = std::move(conditions);
        }

        while (getline(is, line))
        {
            line = line.substr(1, line.size() - 2);
            std::istringstream iss{line};
            Part part;
            std::string def;
            while (getline(iss, def, ','))
            {
                char field = def[0];
                int val = std::stoi(std::string{def.substr(2)});
                part.params[field] = val;
            }
            parts.push_back(part);
        }
    }

    int Task1() const
    {
        using EvalT = std::function<bool(const Part &p)>;
        std::unordered_map<std::string, EvalT> evals;
        evals["A"] = [](const Part &p) { return true; };
        evals["R"] = [](const Part &p) { return false; };

        for (const auto &rule : rules)
        {
            evals[rule.first] = [&](const Part &p) {
                for (auto &condition : rule.second)
                {
                    switch (condition.op)
                    {
                    case 0:
                        return evals.at(condition.next)(p);
                    case '<':
                        if (p.params.at(condition.field) < condition.val)
                            return evals.at(condition.next)(p);
                        break;
                    case '>':
                        if (p.params.at(condition.field) > condition.val)
                            return evals.at(condition.next)(p);
                        break;
                    }
                }
                assert(false);
                return false;
            };
        }

        int sum{};
        for (const auto &p : parts)
        {
            if (evals.at("in")(p))
                sum += p.GetSum();
        }
        return sum;
    }

    using RangeT = std::pair<int64_t, int64_t>;
    using RangesT = std::unordered_map<char, RangeT>;

    int64_t Task2Impl(std::string name, RangesT ranges) const
    {
        if (name == "R")
            return 0;
        if (name == "A")
        {
            auto rx = ranges.at('x');
            auto rm = ranges.at('m');
            auto ra = ranges.at('a');
            auto rs = ranges.at('s');
            return (rx.second - rx.first + 1) * (rm.second - rm.first + 1) * (ra.second - ra.first + 1) * (rs.second - rs.first + 1);
        }

        int64_t sum{};
        for (const auto &condition : rules.at(name))
        {
            if (!condition.op)
            {
                sum += Task2Impl(condition.next, ranges);
                continue;
            }
            auto range = ranges.at(condition.field);
            if (range.first < condition.val && condition.val < range.second)
            {
                RangesT new_ranges{ranges};
                if (condition.op == '<')
                {
                    new_ranges[condition.field].second = condition.val - 1;
                    ranges[condition.field].first = condition.val;
                }
                else
                {
                    new_ranges[condition.field].first = condition.val + 1;
                    ranges[condition.field].second = condition.val;
                }
                sum += Task2Impl(condition.next, new_ranges);
            }
        }
        return sum;
    }

    int64_t Task2() const
    {
        RangesT ranges = {{'x', {1, 4000}}, {'m', {1, 4000}}, {'a', {1, 4000}}, {'s', {1, 4000}}};
        return Task2Impl("in", ranges);
    }
};

suite s = [] {
    "19"_test = [] {
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
        expect(eq(167409079868000ll, test1.Task2()));

        Workflows task{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", task.Task1());
        Printer::Print(__FILE__, "2", task.Task2());
    };
};

} //namespace;
