#include "../test.hpp"
#include <functional>
#include <unordered_set>
#include <fstream>

namespace {

class Comp
{
public:
    Comp(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            int arg{};
            if (1 == sscanf(line.c_str(), "acc %d", &arg))
                _program.push_back({Op::Acc, arg});
            else if (1 == sscanf(line.c_str(), "jmp %d", &arg))
                _program.push_back({Op::Jmp, arg});
            else // nop
                _program.push_back({Op::Nop, arg});
        }
    }

    bool Run()
    {
        _ip = 0;
        _a = 0;
        std::unordered_set<size_t> history;
        while (!history.contains(_ip))
        {
            history.insert(_ip);
            if (_ip == _program.size())
                return true;  // normal finish
            Cmd cmd = _program.at(_ip);
            Execute(cmd);
        }
        return false;  // loop detected
    }

    bool FixProgram()
    {
        for (size_t i = 0; i < _program.size(); ++i)
        {
            auto old_cmd = _program[i];
            if (old_cmd.op == Op::Jmp)
                _program[i].op = Op::Nop;
            else if (old_cmd.op == Op::Nop)
                _program[i].op = Op::Jmp;

            if (old_cmd.op != _program[i].op)
            {
                if (Run())
                    return true;
            }

            _program[i] = old_cmd;
        }
        return false;
    }

    int GetAcc() const { return _a; }

private:
    enum class Op { Acc, Jmp, Nop, };
    struct Cmd
    {
        Op op;
        int arg;
    };
    std::vector<Cmd> _program;
    size_t _ip = 0;
    int _a = 0;

    void Execute(Cmd cmd)
    {
        switch (cmd.op)
        {
        case Op::Acc:
            _a += cmd.arg;
            ++_ip;
            break;
        case Op::Jmp:
            _ip += cmd.arg;
            break;
        case Op::Nop:
            ++_ip;
            break;
        }
    }
};

using namespace boost::ut;

suite s = [] {
    "08"_test = [] {
        {
            const char *const TEST = ""
                R"(nop +0
acc +1
jmp +4
acc +3
jmp -3
acc -99
acc +1
jmp -4
acc +6)";
            Comp c{std::istringstream{TEST}};
            expect(!c.Run());
            expect(5_i == c.GetAcc());
            expect(c.FixProgram());
            expect(8_i == c.GetAcc());
        }

        Comp comp{std::ifstream{INPUT}};
        expect(!comp.Run());
        Printer::Print(__FILE__, "1", comp.GetAcc());
        expect(comp.FixProgram());
        Printer::Print(__FILE__, "2", comp.GetAcc());
    };
};

} //namespace;
