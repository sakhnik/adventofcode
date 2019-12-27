#include <doctest/doctest.h>
#include "IntCode.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <set>
#include <unordered_set>


std::string Execute(std::string command, IntCodeB &prog)
{
    std::string output;

    if (command.back() != '\n')
        command.push_back('\n');
    size_t idx{0};
    int input{command[idx++]};

    while (true)
    {
        auto r = prog.Advance(input);
        switch (prog.GetState())
        {
        case IntCodeB::S_RUN:
            break;
        case IntCodeB::S_HALT:
            return output;
        case IntCodeB::S_INPUT:
            if (idx >= command.size())
                return output;
            input = command[idx++];
            break;
        case IntCodeB::S_OUTPUT:
            output.push_back(r.convert_to<char>());
            break;
        }
    }
}

TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCodeB prog{ifs};

    std::unordered_set<std::string> visited;

    struct Path
    {
        std::string command;
        bool isMotion = false;
    };

    std::vector<Path> commands{{""}};
    std::vector<std::string> items;
    std::string roomName;

    while (!commands.empty())
    {
        auto [cmd,isMotion] = commands.back();
        commands.pop_back();
        std::cout << cmd << std::endl;

        if (isMotion)
        {
            if (cmd == "north")
                commands.push_back({"south"});
            else if (cmd == "south")
                commands.push_back({"north"});
            else if (cmd == "east")
                commands.push_back({"west"});
            else if (cmd == "west")
                commands.push_back({"east"});
        }

        std::string output = Execute(cmd, prog);
        std::cout << output;

        const std::regex ename("== [^=]+==");
        std::smatch mname;
        if (std::regex_search(output, mname, ename))
            roomName = mname.str();

        if (visited.insert(roomName).second)
        {
            const std::regex e("\n- ([a-z0-9 ]+)");
            for (std::regex_token_iterator<std::string::iterator> rit(output.begin(), output.end(), e, 1), rend;
                 rit != rend; ++rit)
            {
                auto feature = rit->str();
                if (feature == "north")
                    commands.push_back({"north", true});
                else if (feature == "south")
                    commands.push_back({"south", true});
                else if (feature == "west")
                    commands.push_back({"west", true});
                else if (feature == "east")
                    commands.push_back({"east", true});
                else
                {
                    static const std::unordered_set<std::string> IGNORES{
                        "infinite loop", "giant electromagnet", "photons",
                        "molten lava", "escape pod"
                    };
                    if (!IGNORES.count(feature))
                    {
                        commands.push_back({"take " + feature});
                        items.push_back(feature);
                    }
                }
            }
        }
    }

    std::string cmd;
    while (getline(std::cin, cmd))
    {
        std::cout << Execute(cmd, prog);
    }
}
