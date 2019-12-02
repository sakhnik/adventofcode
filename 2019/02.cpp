#include <doctest/doctest.h>
#include <fstream>
#include <unordered_map>
#include <iostream>


using MemoryT = std::unordered_map<int, int>;

int Run(MemoryT memory, int noun, int verb)
{
    // Tweak the initials as required in the task 1
    memory[1] = noun;
    memory[2] = verb;

    // Run the memory until it halts
    int ip{0};

    while (true)
    {
        int opcode = memory[ip++];
        if (opcode == 99)
        {
            break;
        }
        int a = memory[ip++];
        int b = memory[ip++];
        int c = memory[ip++];
        if (opcode == 1)
        {
            memory[c] = memory[a] + memory[b];
        }
        else if (opcode == 2)
        {
            memory[c] = memory[a] * memory[b];
        }
        else
        {
            std::cout << "HCF" << std::endl;
            break;
        }
    }

    return memory[0];
}

TEST_CASE(TEST_NAME)
{
    // The memory numbers
    std::unordered_map<int, int> memory;

    // Input the memory from the file
    std::ifstream ifs{INPUT};
    int idx{0};
    while (ifs)
    {
        int val{};
        char comma;
        ifs >> val >> comma;
        memory[idx++] = val;
    }

    // Output the number in the first register
    MESSAGE(Run(memory, 12, 2));

    // Task 2
    for (int noun = 0; noun < 100; ++noun)
    {
        for (int verb = 0; verb < 100; ++verb)
        {
            if (19690720 == Run(memory, noun, verb))
            {
                MESSAGE(noun * 100 + verb);
                break;
            }
        }
    }
}
