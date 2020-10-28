#include "IntCode.h"
#include <fstream>
#include <regex>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include "../test.hpp"

namespace {

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

using namespace boost::ut;

suite s = [] {
    "2019-25"_test = [] {
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

        const std::unordered_map<std::string, std::string> BACK_MOVES{
            {"north", "south"},
            {"south", "north"},
            {"east", "west"},
            {"west", "east"}};
        std::vector<std::string> pathToSecurity;

        while (!commands.empty())
        {
            auto [cmd, isMotion] = commands.back();
            commands.pop_back();
            std::cout << cmd << std::endl;

            if (isMotion)
                commands.push_back({BACK_MOVES.at(cmd)});

            std::string output = Execute(cmd, prog);
            std::cout << output;

            const std::regex ename("== [^=]+==");
            std::smatch mname;
            if (std::regex_search(output, mname, ename))
            {
                roomName = mname.str();
                if (roomName == "== Security Checkpoint ==" && pathToSecurity.empty())
                {
                    // Remember the path to the security checkpoint
                    for (auto it = commands.begin(); it != commands.end(); ++it)
                    {
                        if (!it->isMotion)
                        {
                            auto move = BACK_MOVES.find(it->command);
                            if (move != BACK_MOVES.end())
                                pathToSecurity.push_back(move->second);
                        }
                    }
                }
            }

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
                            "molten lava", "escape pod"};
                        if (!IGNORES.count(feature))
                        {
                            commands.push_back({"take " + feature});
                            items.push_back(feature);
                        }
                    }
                }
            }
        }

        // Go to the security checkpoint
        for (const auto &s : pathToSecurity)
        {
            std::cout << Execute(s, prog);
        }

        // Pick a correct combination of items
        int combination{(1 << items.size()) - 1};
        for (size_t i = 0, n = 1 << items.size(); i < n; ++i)
        {
            for (size_t j = 0; j < items.size(); ++j)
            {
                bool needJ = (i >> j) & 1;
                bool hasJ = (combination >> j) & 1;
                if (needJ == hasJ)
                    continue;
                std::string cmd = (hasJ ? "drop " : "take ") + items[j] + "\n";
                std::cout << cmd << Execute(cmd, prog);
            }
            combination = i;

            auto output = Execute("north", prog);
            std::cout << output;
            static const std::regex e("You may proceed.");
            if (std::regex_search(output, e))
                break;
        }

        //std::string cmd;
        //while (getline(std::cin, cmd))
        //{
        //    std::cout << Execute(cmd, prog);
        //}
    };
};

} //namespace;