#include "Asm.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>


class MessageQueue
{
public:
    size_t Size() const { return _q.size(); }

    void Send(int64_t v)
    {
        {
            std::lock_guard<std::mutex> l(_m);
            _q.push(v);
        }
        _cv.notify_one();
        ++_sends;
    }

    int64_t Receive()
    {
        std::unique_lock<std::mutex> l(_m);
        if (!_cv.wait_for(l, std::chrono::seconds(1), [&] { return !_q.empty(); }))
            throw "Deadlock";
        auto v = _q.front();
        _q.pop();
        return v;
    }

    unsigned GetSends() const { return _sends; }

private:
    std::queue<int64_t> _q;
    std::mutex _m;
    std::condition_variable _cv;
    unsigned _sends = 0;
};

void Execute(int64_t id,
             const ProgramT &program,
             MessageQueue &in,
             MessageQueue &out,
             bool task1 = false,
             bool debug = false)
{
    std::unordered_map<char, int64_t> regs;
    regs['p'] = id;

    auto get_value = [&](Arg a) {
        if (a.is_reg)
            return regs[a.a.reg];
        return a.a.num;
    };

    auto print_arg = [&](std::ostream &os, Arg a) -> std::ostream& {
        os << ' ';
        if (a.is_reg)
            return os << a.a.reg << "(" << regs[a.a.reg] << ")";
        return os << a.a.num;
    };

    for (size_t i = 0; i < program.size(); ++i)
    {
        using Op = Instr::Op;
        auto instr = program[i];
        auto X = instr.X;
        auto Y = instr.Y;

        switch (instr.op)
        {
        case Op::Snd:
            //snd X plays a sound with a frequency equal to the value of X.
            if (debug)
            {
                std::cout << i << ": snd";
                print_arg(std::cout, X) << std::endl;
            }
            out.Send(get_value(X));
            break;
        case Op::Set:
            //set X Y sets register X to the value of Y.
            if (debug)
            {
                std::cout << i << ": set " << X.a.reg;
                print_arg(std::cout, Y) << std::endl;
            }
            regs[X.a.reg] = get_value(Y);
            break;
        case Op::Add:
            //add X Y increases register X by the value of Y.
            if (debug)
            {
                std::cout << i << ": add " << X.a.reg;
                print_arg(std::cout, Y) << std::endl;
            }
            regs[X.a.reg] += get_value(Y);
            break;
        case Op::Mul:
            //mul X Y sets register X to the result of multiplying the value contained in register X by the value of Y.
            if (debug)
            {
                std::cout << i << ": mul " << X.a.reg;
                print_arg(std::cout, Y) << std::endl;
            }
            regs[X.a.reg] *= get_value(Y);
            break;
        case Op::Mod:
            //mod X Y sets register X to the remainder of dividing the value contained in register X by the value of Y (that is, it sets X to the result of X modulo Y).
            if (debug)
            {
                std::cout << i << ": mod " << X.a.reg;
                print_arg(std::cout, Y) << std::endl;
            }
            regs[X.a.reg] %= get_value(Y);
            break;
        case Op::Rcv:
            //rcv X recovers the frequency of the last sound played, but only when the value of X is not zero. (If it is zero, the command does nothing.)
            if (debug)
            {
                std::cout << i << ": rcv";
                print_arg(std::cout, X) << std::endl;
            }
            if (task1)
            {
                // First half of the task: return after first receive
                if (get_value(X) != 0)
                    return;
            }
            else
            {
                regs[X.a.reg] = in.Receive();
            }
            break;
        case Op::Jgz:
            //jgz X Y jumps with an offset of the value of Y, but only if the value of X is greater than zero. (An offset of 2 skips the next instruction, an offset of -1 jumps to the previous instruction, and so on.)
            if (debug)
            {
                std::cout << i << ": jgz";
                print_arg(std::cout, X);
                print_arg(std::cout, Y) << std::endl;
            }
            if (get_value(X) > 0)
                i += get_value(Y) - 1;
            break;
        default:
            break;
        }
    }
}

int64_t Task1(const ProgramT &program)
{
    MessageQueue q;
    Execute(0, program, q, q, true);
    while (q.Size() > 1)
        q.Receive();
    return q.Receive();
}

int64_t Task2(const ProgramT &program)
{
    MessageQueue q1, q2;
    std::thread t([&] {
        try
        {
            Execute(0, program, q1, q2);
        }
        catch (...)
        {
        }
    });
    try
    {
        Execute(1, program, q2, q1);
    }
    catch (...)
    {
    }
    t.join();
    return q1.GetSends();
}


TEST_CASE("1")
{
    const char *const program = R"(set a 1
add a 2
mul a a
mod a 5
snd a
set a 0
rcv a
jgz a -1
set a 1
jgz a -2)";
    auto test = Parse(std::istringstream(program));
    REQUIRE(Task1(test) == 4);
}

TEST_CASE("2")
{
    const char *const program = R"(snd 1
snd 2
snd p
rcv a
rcv b
rcv c
rcv d)";
    auto test = Parse(std::istringstream(program));
    REQUIRE(Task2(test) == 3);
}

TEST_CASE("main")
{
    auto program = Parse(std::ifstream(INPUT));
    std::cout << Task1(program) << std::endl;
    std::cout << Task2(program) << std::endl;
}
