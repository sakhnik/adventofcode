#include <doctest/doctest.h>
#include "IntCode.h"
#include <iostream>
#include <fstream>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCodeB prog{ifs};

    std::string command;
    size_t idx{0};
    int input{};
    bool exit{false};

    while (!exit)
    {
        auto r = prog.Advance(input);
        switch (prog.GetState())
        {
        case prog.S_RUN:
            break;
        case prog.S_HALT:
            exit = true;
            break;
        case prog.S_INPUT:
            if (idx >= command.size())
            {
                if (!getline(std::cin, command))
                    exit = true;
                command.push_back('\n');
                idx = 0;
            }
            input = command[idx++];
            break;
        case prog.S_OUTPUT:
            std::cout << char(r);
            break;
        }
    }
}
