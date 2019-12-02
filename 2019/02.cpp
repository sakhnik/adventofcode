#include <doctest/doctest.h>
#include <fstream>
#include <unordered_map>
#include <iostream>


TEST_CASE(TEST_NAME)
{
    // The program numbers
    std::unordered_map<int, int> program;

    // Input the program from the file
    std::ifstream ifs{INPUT};
    int idx{0};
    while (ifs)
    {
        int val{};
        char comma;
        ifs >> val >> comma;
        program[idx++] = val;
    }

    // Tweak the initials as required in the task 1
    program[1] = 12;
    program[2] = 2;

    // Run the program until it halts
    int pc{0};

    while (true)
    {
        int opcode = program[pc++];
        if (opcode == 99)
        {
            break;
        }
        int a = program[pc++];
        int b = program[pc++];
        int c = program[pc++];
        if (opcode == 1)
        {
            program[c] = program[a] + program[b];
        }
        else if (opcode == 2)
        {
            program[c] = program[a] * program[b];
        }
        else
        {
            std::cout << "HCF" << std::endl;
            break;
        }
    }

    // Output the number in the first register
    MESSAGE(program[0]);
}
