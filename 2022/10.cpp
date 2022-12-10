#include "../test.hpp"
#include <fstream>

namespace {

struct IObserver
{
    virtual void Tick(int cycle, int x) = 0;
};

struct Task1 : IObserver
{
    size_t strength = 0;

    void Tick(int cycle, int x) override
    {
        if (cycle == 20)
            strength += 20 * x;
        else if (cycle == 60)
            strength += 60 * x;
        else if (cycle == 100)
            strength += 100 * x;
        else if (cycle == 140)
            strength += 140 * x;
        else if (cycle == 180)
            strength += 180 * x;
        else if (cycle == 220)
            strength += 220 * x;
    }
};

struct Task2 : Task1
{
    std::string screen;

    void Tick(int cycle, int x) override
    {
        Task1::Tick(cycle, x);

        int col = (cycle - 1) % 40;
        screen.push_back((col >= x - 1 && col <= x + 1) ? '#' : '.');
        if (col == 39)
            screen.push_back('\n');
    }
};

struct Machine
{
    IObserver &observer;
    int cycle = 0;
    int x = 1;

    Machine(IObserver &observer) : observer{observer} { }

    void AddX(int val)
    {
        observer.Tick(++cycle, x);
        observer.Tick(++cycle, x);
        x += val;
    }

    void Noop()
    {
        observer.Tick(++cycle, x);
    }
};

std::pair<size_t, std::string> Execute(std::istream &&is)
{
    Task2 task;
    Machine machine{task};
    std::string line;
    while (std::getline(is, line))
    {
        int val{};
        if (1 == sscanf(line.c_str(), "addx %d", &val))
            machine.AddX(val);
        else if (line == "noop")
            machine.Noop();
    }
    return {task.strength, task.screen};
}

const char *const TEST = R"(addx 15
addx -11
addx 6
addx -3
addx 5
addx -1
addx -8
addx 13
addx 4
noop
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx -35
addx 1
addx 24
addx -19
addx 1
addx 16
addx -11
noop
noop
addx 21
addx -15
noop
noop
addx -3
addx 9
addx 1
addx -3
addx 8
addx 1
addx 5
noop
noop
noop
noop
noop
addx -36
noop
addx 1
addx 7
noop
noop
noop
addx 2
addx 6
noop
noop
noop
noop
noop
addx 1
noop
noop
addx 7
addx 1
noop
addx -13
addx 13
addx 7
noop
addx 1
addx -33
noop
noop
noop
addx 2
noop
noop
noop
addx 8
noop
addx -1
addx 2
addx 1
noop
addx 17
addx -9
addx 1
addx 1
addx -3
addx 11
noop
noop
addx 1
noop
addx 1
noop
noop
addx -13
addx -19
addx 1
addx 3
addx 26
addx -30
addx 12
addx -1
addx 3
addx 1
noop
noop
noop
addx -9
addx 18
addx 1
addx 2
noop
noop
addx 9
noop
noop
noop
addx -1
addx 2
addx -37
addx 1
addx 3
noop
addx 15
addx -21
addx 22
addx -6
addx 1
noop
addx 2
addx 1
noop
addx -10
noop
noop
addx 20
addx 1
addx 2
addx 2
addx -6
addx -11
noop
noop
noop)";

using namespace boost::ut;

const char *const TEST_RES =
    "##..##..##..##..##..##..##..##..##..##..\n"
    "###...###...###...###...###...###...###.\n"
    "####....####....####....####....####....\n"
    "#####.....#####.....#####.....#####.....\n"
    "######......######......######......####\n"
    "#######.......#######.......#######.....\n";

suite s = [] {
    "2022-10"_test = [] {
        auto test = Execute(std::istringstream{TEST});
        expect(13140_u == test.first);
        expect(TEST_RES == test.second);

        auto res = Execute(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", res.first);
        Printer::Print(__FILE__, "2", "\n" + res.second);
    };
};

} //namespace;
