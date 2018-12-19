#include <doctest/doctest.h>
#include <array>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>


namespace {

class Cpu
{
public:
    using IntT = int;

    Cpu(std::istream &&is)
    {
        std::string word;
        IntT a{}, b{}, c{};
        is >> word >> a;
        _ip = &_regs[a];

        static const std::string opcodes[] = {
            "addr", "addi", "mulr", "muli",
            "banr", "bani", "borr", "bori",
            "setr", "seti", "gtir", "gtri",
            "gtrr", "eqir", "eqri", "eqrr",
        };

        while (is && (is >> word >> a >> b >> c))
        {
            auto it = std::find(begin(opcodes), end(opcodes), word);
            _program.push_back({IntT(it - begin(opcodes)), a, b, c});
        }
    }

    void Step()
    {
        const auto &instr = _program.at(*_ip);
        _Eval(instr.op, instr.a, instr.b, instr.c);
        ++*_ip;
    }

    void Execute()
    {
        try
        {
            while (true)
            {
                Step();
            }
        }
        catch (...)
        {
        }
    }

    using RegsT = std::array<IntT, 6>;
    const RegsT& GetRegs() const
    {
        return _regs;
    }

private:
    RegsT _regs = {};
    IntT *_ip{};

    struct _Instruction
    {
        IntT op, a, b, c;
    };
    std::vector<_Instruction> _program;


    void _Eval(IntT op, IntT a, IntT b, IntT c)
    {
        switch (op)
        {
        case 0:  _regs[c] = _regs[a] + _regs[b];  break;  // addr
        case 1:  _regs[c] = _regs[a] + b;         break;  // addi
        case 2:  _regs[c] = _regs[a] * _regs[b];  break;  // mulr
        case 3:  _regs[c] = _regs[a] * b;         break;  // muli
        case 4:  _regs[c] = _regs[a] & _regs[b];  break;  // banr
        case 5:  _regs[c] = _regs[a] & b;         break;  // bani
        case 6:  _regs[c] = _regs[a] | _regs[b];  break;  // borr
        case 7:  _regs[c] = _regs[a] | b;         break;  // bori
        case 8:  _regs[c] = _regs[a];             break;  // setr
        case 9:  _regs[c] = a;                    break;  // seti
        case 10: _regs[c] = a > _regs[b];         break;  // gtir
        case 11: _regs[c] = _regs[a] > b;         break;  // gtri
        case 12: _regs[c] = _regs[a] > _regs[b];  break;  // gtrr
        case 13: _regs[c] = a == _regs[b];        break;  // eqir
        case 14: _regs[c] = _regs[a] == b;        break;  // eqri
        case 15: _regs[c] = _regs[a] == _regs[b]; break;  // eqrr
        }
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        Cpu cpu(std::istringstream{
            "#ip 0\n"
            "seti 5 0 1\n"
            "seti 6 0 2\n"
            "addi 0 1 0\n"
            "addr 1 2 3\n"
            "setr 1 0 0\n"
            "seti 8 0 4\n"
            "seti 9 0 5\n"
            });
        REQUIRE(Cpu::RegsT{0, 0, 0, 0, 0, 0} == cpu.GetRegs());
        cpu.Step();
        REQUIRE(Cpu::RegsT{1, 5, 0, 0, 0, 0} == cpu.GetRegs());
        cpu.Step();
        REQUIRE(Cpu::RegsT{2, 5, 6, 0, 0, 0} == cpu.GetRegs());
        cpu.Step();
        REQUIRE(Cpu::RegsT{4, 5, 6, 0, 0, 0} == cpu.GetRegs());
        cpu.Step();
        REQUIRE(Cpu::RegsT{6, 5, 6, 0, 0, 0} == cpu.GetRegs());
    }

    SUBCASE("task1") {
        Cpu cpu(std::ifstream{INPUT});
        cpu.Execute();
        MESSAGE(cpu.GetRegs()[0]);
    }
}
