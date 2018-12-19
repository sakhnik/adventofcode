#include <doctest/doctest.h>
#include <array>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>


namespace {

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

    SUBCASE("task2") {
        //Cpu cpu(std::ifstream{INPUT}, {1, 0, 0, 0, 0, 0});
        //cpu.Execute();
        //MESSAGE(cpu.GetRegs()[0]);
        //cpu.Print();

        //           +- 0. jump 17             // r2 = r2 + 16
        //         +-|> 1. r1 = 1
        //     +---|-|> 2. r3 = 1
        //     | +-|-|> 3. r5 = r1 * r3
        //     | | | |  4. r5 = r5 == r4
        //     | | | |  5. jump +[r5 + 1]      // r2 = r5 + r2
        //     | | | |  6. jump +2             // r2 = r2 + 1
        //     | | | |  7. r0 += r1            // r0 = r1 + r0
        //     | | | |  8. ++r3                // r3 = r3 + 1
        //     | | | |  9. r5 = r3 > r4
        //     | | | |  10. jump +[r5 + 1]     // r2 = r2 + r5
        //     | +-|-|- 11. jump 3             // r2 = 2
        //     |   | |  12. ++r1               // r1 = r1 + 1
        //     |   | |  13. r5 = r1 > r4
        //     |   | |  14. jump +[r5 + 1]     // r2 = r5 + r2
        //     +---|-|- 15. jump 2             // r2 = 1
        //         | |  16. halt               // r2 = r2 * r2
        //         | +> 17. r4 += 2            // r4 = r4 + 2
        //         |    18. r4 = r4 * r4
        //         |    19. r4 *= 19           // r4 = r2 * r4
        //         |    20. r4 *= 11           // r4 = r4 * 11
        //         |    21. ++r5               // r5 = r5 + 1
        //         |    22. r5 *= 22           // r5 = r5 * r2
        //         |    23. r5 += 17           // r5 = r5 + 17
        //         |    24. r4 += r5           // r4 = r4 + r5
        //         |    25. jump +[r0 + 1]     // r2 = r2 + r0
        //         +--- 26. jump 1             // r2 = 0
        //              27. r5 = 27            // r5 = r2
        //              28. r5 *= 28           // r5 = r5 * r2
        //              29. r5 += 29           // r5 = r2 + r5
        //              30. r5 *= 30           // r5 = r2 * r5
        //              31. r5 *= 14           // r5 = r5 * 14
        //              32. r5 *= 32           // r5 = r5 * r2
        //              33. r4 += r5           // r4 = r4 + r5
        //              34. r0 = 0
        //              35. jump 1             // r2 = 0
        //
        //  r4 -> (4*19*11) + (22 + 17)
        //  r4 -> (27 * 28 + 29) * 30 * 14 * 32 + r4
        //
        //  r1 = 1
        //  while (r1 <= r4):
        //    for r3 = 1; r3 <= r4; ++r3:
        //      if r3 * r1 == r4:
        //        r0 += r1
        //

        auto calc = [](int64_t r4) {
            int64_t r0 = 0;

            for (int64_t r1 = 1; r1 <= r4; ++r1)
            {
                for (int64_t r3 = 1; r3 <= r4; ++r3)
                {
                    auto m = r3 * r1;
                    if (m == r4)
                    {
                        r0 += r1;
                    }
                    if (m > r4)
                    {
                        break;
                    }
                }
            }

            return r0;
        };

        int64_t r4 = 4ll*19*11 + 22 + 17;
        REQUIRE(1248 == calc(r4));
        r4 += (27 * 28 + 29) * 30 * 14 * 32;
        MESSAGE(calc(r4));
    }
}
