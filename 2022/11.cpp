#include "../test.hpp"
#include <fstream>

namespace {

struct Monkey
{
    std::vector<size_t> items;
    std::function<size_t(size_t)> operation;
    size_t divisor{};
    size_t throw_true{};
    size_t throw_false{};
    size_t inspected{};
};

using MonkeysT = std::vector<Monkey>;

MonkeysT Parse(std::istream &&is)
{
    MonkeysT monkeys;
    while (is)
    {
        std::string line;
        std::getline(is, line);   // Monkey 0
        if (line.empty())
            continue;
 
        Monkey monkey;

        // Starting items
        std::getline(is, line);
        line = line.substr(line.find(':') + 2);
        std::istringstream iss{line};
        size_t item{};
        while (iss >> item)
        {
            monkey.items.push_back(item);
            iss >> line;
        }

        // Operation
        std::getline(is, line);
        line = line.substr(line.find(':') + 2);
        int val{};
        if (1 == sscanf(line.c_str(), "new = old + %d", &val))
            monkey.operation = [val](size_t old) { return old + val; };
        else if (1 == sscanf(line.c_str(), "new = old * %d", &val))
            monkey.operation = [val](size_t old) { return old * val; };
        else if (line == "new = old * old")
            monkey.operation = [](size_t old) { return old * old; };
        else
            throw std::runtime_error("Failed to parse operation");

        // Test
        std::getline(is, line);
        line = line.substr(line.find(':') + 2);
        sscanf(line.c_str(), "divisible by %ld", &monkey.divisor);
        std::getline(is, line);
        line = line.substr(line.find(':') + 2);
        sscanf(line.c_str(), "throw to monkey %ld", &monkey.throw_true);
        std::getline(is, line);
        line = line.substr(line.find(':') + 2);
        sscanf(line.c_str(), "throw to monkey %ld", &monkey.throw_false);

        monkeys.push_back(monkey);
    }
    return monkeys;
}

void Run(MonkeysT &monkeys, bool relax)
{
    int lcm{1};
    for (const auto &monkey : monkeys)
        lcm *= monkey.divisor;

    for (auto &monkey : monkeys)
    {
        for (auto &item : monkey.items)
        {
            item = (monkey.operation(item) % lcm);
            if (relax)
                item /= 3;
            int next = (0 == item % monkey.divisor) ? monkey.throw_true : monkey.throw_false;
            monkeys[next].items.push_back(item);
            ++monkey.inspected;
        }
        monkey.items.clear();
    }
}

size_t CalcBusiness(MonkeysT &monkeys)
{
    std::vector<size_t> champs;
    champs.reserve(monkeys.size());
    for (const auto &monkey : monkeys)
        champs.push_back(monkey.inspected);
    std::nth_element(champs.begin(), champs.begin() + 1, champs.end(), std::greater<size_t>());
    return champs[0] * champs[1];
}

size_t Task1(MonkeysT &monkeys)
{
    for (int i = 0; i < 20; ++i)
        Run(monkeys, true);
    return CalcBusiness(monkeys);
}

size_t Task2(MonkeysT &monkeys)
{
    for (int i = 0; i < 10000; ++i)
        Run(monkeys, false);
    return CalcBusiness(monkeys);
}

const char *const TEST = R"(Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1)";

using namespace boost::ut;

suite s = [] {
    "2022-11"_test = [] {
        auto test = Parse(std::istringstream{TEST});
        expect(10605_u == Task1(test));
        auto test2 = Parse(std::istringstream{TEST});
        expect(2713310158_u == Task2(test2));

        auto monkeys = Parse(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", Task1(monkeys));
        auto monkeys2 = Parse(std::ifstream{INPUT});
        Printer::Print(__FILE__, "2", Task2(monkeys2));
    };
};

} //namespace;
