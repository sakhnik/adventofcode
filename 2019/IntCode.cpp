#include "IntCode.h"
#include <iostream>


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

void IntCode::Run()
{
    // Run the memory until it halts
    int ip{0};

    while (true)
    {
        int opcode = _memory[ip++];
        if (opcode == 99)
        {
            break;
        }
        int a = _memory[ip++];
        int b = _memory[ip++];
        int c = _memory[ip++];
        if (opcode == 1)
        {
            _memory[c] = _memory[a] + _memory[b];
        }
        else if (opcode == 2)
        {
            _memory[c] = _memory[a] * _memory[b];
        }
        else
        {
            std::cout << "HCF" << std::endl;
            break;
        }
    }
}
