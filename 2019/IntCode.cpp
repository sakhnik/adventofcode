#include "IntCode.h"


IntCode::IntCode(std::istream &is)
{
    int idx{0};
    while (is)
    {
        int val{};
        char comma;
        is >> val >> comma;
        _memory[idx++] = val;
    }
}

void IntCode::Run(std::istream &is, std::ostream &os)
{
    // Run the memory until it halts
    int ip{0};

    while (true)
    {
        int cmd = _memory[ip++];
        int opcode = cmd % 100;
        cmd /= 100;

        auto getArg = [&]() -> int& {
            int mode = cmd % 10;
            cmd /= 10;
            switch (mode)
            {
            case 0:
                return _memory[_memory[ip++]];
            case 1:
                return _memory[ip++];
            default:
                throw "Mode not implemented";
            }
        };

        switch (opcode)
        {
        case 1:
            {
                auto a = getArg();
                auto b = getArg();
                getArg() = a + b;
            }
            break;
        case 2:
            {
                auto a = getArg();
                auto b = getArg();
                getArg() = a * b;
            }
            break;
        case 3:
            {
                int val{};
                is >> val;
                getArg() = val;
            }
            break;
        case 4:
            {
                os << getArg();
            }
            break;
        case 99:
            return;
        default:
            std::cout << "HCF" << std::endl;
            break;
        }
    }
}
