#include "IntCode.h"
#include <fstream>
#include <deque>
#include "../test.hpp"

namespace {

using namespace boost::ut;

suite s = [] {
    "2019-23"_test = [] {
        std::ifstream ifs{INPUT};
        IntCodeB prog{ifs};

        struct Node
        {
            IntCodeB prog;
            std::deque<BigIntT> input;
        };

        size_t queue_size{};
        std::vector<Node> nodes(50, {prog});
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            nodes[i].input.push_front(0);
            nodes[i].input.push_back(i);
            queue_size += 2;
        }

        bool firstReported{false};
        BigIntT nat_y_prev{-1};
        BigIntT nat_x{}, nat_y{};

        bool exit{false};
        while (!exit)
        {
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                auto &n = nodes[i];

                BigIntT v = -1;
                if (!n.input.empty())
                {
                    v = n.input.front();
                    n.input.pop_front();
                    --queue_size;
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
                    assert(n.prog.GetState() == IntCode::S_OUTPUT);
                    auto y = n.prog.Advance(0);
                    assert(n.prog.GetState() == IntCode::S_OUTPUT);
                    n.input.push_front(0);
                    ++queue_size;
                    if (r == 255)
                    {
                        if (!firstReported)
                        {
                            Printer::Print(__FILE__, "1", y.convert_to<std::string>());
                            firstReported = true;
                        }
                        nat_x = x;
                        nat_y = y;
                    }
                    if (r < nodes.size())
                    {
                        auto addr = r.convert_to<int>();
                        nodes[addr].input.push_back(x);
                        nodes[addr].input.push_back(y);
                        queue_size += 2;
                    }
                }
                break;
                }
            }

            if (!queue_size)
            {
                if (nat_y_prev == nat_y)
                {
                    Printer::Print(__FILE__, "2", nat_y.convert_to<std::string>());
                    exit = true;
                    break;
                }
                nodes[0].input.push_back(nat_x);
                nodes[0].input.push_back(nat_y);
                queue_size += 2;
                nat_y_prev = nat_y;
            }
        }
    };
};

} //namespace;