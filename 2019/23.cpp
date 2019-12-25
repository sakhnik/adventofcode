#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>
#include <deque>
#include <iostream>


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCodeB prog{ifs};

    struct Node
    {
        IntCodeB prog;
        std::deque<BigIntT> input;
    };

    std::vector<Node> nodes(50, {prog});
    for (int i = 0; i < nodes.size(); ++i)
    {
        nodes[i].input.push_front(0);
        nodes[i].input.push_back(i);
    }

    bool exit{false};
    while (!exit)
    {
        for (int i = 0; i < nodes.size(); ++i)
        {
            auto &n = nodes[i];

            BigIntT v = -1;
            if (!n.input.empty())
            {
                v = n.input.front();
                n.input.pop_front();
            }

            auto r = n.prog.Advance(v);

            switch (n.prog.GetState())
            {
            case IntCodeB::S_RUN:
                break;
            case IntCodeB::S_HALT:
                exit = true;
                break;
            case IntCodeB::S_INPUT:
                break;
            case IntCodeB::S_OUTPUT:
                {
                    auto x = n.prog.Advance(0);
                    REQUIRE(n.prog.GetState() == IntCode::S_OUTPUT);
                    auto y = n.prog.Advance(0);
                    REQUIRE(n.prog.GetState() == IntCode::S_OUTPUT);
                    n.input.push_front(0);
                    if (r == 255)
                        MESSAGE(y);
                    if (r < nodes.size())
                    {
                        auto addr = r.convert_to<int>();
                        nodes[addr].input.push_back(x);
                        nodes[addr].input.push_back(y);
                    }
                    else
                    {
                        exit = true;
                    }
                }
                break;
            }
        }
    }
}
