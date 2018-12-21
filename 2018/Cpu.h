#pragma once

#include <array>
#include <iostream>
#include <algorithm>
#include <vector>

namespace aoc2018 {

class Cpu
{
public:
    using IntT = int;
    using RegsT = std::array<IntT, 6>;

    Cpu(std::istream &&is, RegsT regs = RegsT{})
        : _regs(regs)
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

    const RegsT& GetRegs() const
    {
        return _regs;
    }

    IntT GetIp() const
    {
        return *_ip;
    }

    void Print() const
    {
        for (size_t i = 0; i < _program.size(); ++i)
        {
            std::cout << i << ". ";
            const auto &instr = _program[i];
            auto a = instr.a;
            auto b = instr.b;
            auto c = instr.c;

            std::cout << 'r' << c << " = ";
            switch (instr.op)
            {
            case 0:  std::cout << 'r' << a << " + " << 'r' << b;  break;
            case 1:  std::cout << 'r' << a << " + " << b;         break;
            case 2:  std::cout << 'r' << a << " * " << 'r' << b;  break;
            case 3:  std::cout << 'r' << a << " * " << b;         break;
            case 4:  std::cout << 'r' << a << " & " << 'r' << b;  break;
            case 5:  std::cout << 'r' << a << " & " << b;         break;
            case 6:  std::cout << 'r' << a << " | " << 'r' << b;  break;
            case 7:  std::cout << 'r' << a << " | " << b;         break;
            case 8:  std::cout << 'r' << a;                       break;
            case 9:  std::cout << a;                              break;
            case 10: std::cout << a << " > " << 'r' << b;         break;
            case 11: std::cout << 'r' << a << " > " << b;         break;
            case 12: std::cout << 'r' << a << " > " << 'r' << b;  break;
            case 13: std::cout << a << " == " << 'r' << b;        break;
            case 14: std::cout << 'r' << a << " == " << b;        break;
            case 15: std::cout << 'r' << a << " == " << 'r' << b; break;
            }
            std::cout << "\n";
        }
    }

private:
    RegsT _regs;
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

} //namespace aoc2018;

