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

    struct Pos
    {
        int x,y;
        bool operator<(const Pos &o) const
        {
            if (x < o.x) return true;
            if (x > o.x) return false;
            return y < o.y;
        }
        bool operator==(const Pos &o) const
        {
            return x == o.x && y == o.y;
        }
    };

    std::set<Pos> visited{{0,0}};

    struct Path
    {
        std::string command;
        Pos pos;
        bool isMotion = false;
    };

    std::vector<Path> commands{{"", {0,0}}};
    std::vector<std::string> items;

    while (!commands.empty())
    {
        auto [cmd,pos,isMotion] = commands.back();
        commands.pop_back();
        std::cout << cmd << std::endl;

        if (isMotion)
        {
            if (cmd == "north")
                commands.push_back({"south", Pos{pos.x, pos.y + 1}});
            else if (cmd == "south")
                commands.push_back({"north", Pos{pos.x, pos.y - 1}});
            else if (cmd == "east")
                commands.push_back({"west", Pos{pos.x - 1, pos.y}});
            else if (cmd == "west")
                commands.push_back({"east", Pos{pos.x + 1, pos.y}});
        }

        std::string output = Execute(cmd, prog);
        std::cout << output;

        const std::regex e("\n- ([a-z0-9 ]+)");
        for (std::regex_token_iterator<std::string::iterator> rit(output.begin(), output.end(), e, 1), rend;
             rit != rend; ++rit)
        {
            auto feature = rit->str();
            if (feature == "north")
            {
                Pos newPos{pos.x, pos.y - 1};
                if (visited.insert(newPos).second)
                    commands.push_back({"north", newPos, true});
            }
            else if (feature == "south")
            {
                Pos newPos{pos.x, pos.y + 1};
                if (visited.insert(newPos).second)
                    commands.push_back({"south", newPos, true});
            }
            else if (feature == "west")
            {
                Pos newPos{pos.x - 1, pos.y};
                if (visited.insert(newPos).second)
                    commands.push_back({"west", newPos, true});
            }
            else if (feature == "east")
            {
                Pos newPos{pos.x + 1, pos.y};
                if (visited.insert(newPos).second)
                    commands.push_back({"east", newPos, true});
            }
            else
            {
                static const std::unordered_set<std::string> IGNORES{
                    "infinite loop", "giant electromagnet", "photons"
                };
                if (!IGNORES.count(feature))
                {
                    commands.push_back({"take " + feature, pos});
                    items.push_back(feature);
                }
            }
        }
    }

    std::cout << items.size() << std::endl;
}
