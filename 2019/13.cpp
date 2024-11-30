#include "IntCode.h"
#include <fstream>
#include "../test.hpp"

namespace {

using namespace boost::ut;

suite s = [] {
    "13"_test = [] {
        std::ifstream ifs{INPUT};
        IntCode prog{ifs};

        {
            IntCode p1{prog};
            int count{0};

            while (p1.GetState() != p1.S_HALT)
            {
                /*auto x =*/p1.Advance(0);
                if (p1.GetState() == p1.S_HALT)
                    break;
                expect(p1.GetState() == p1.S_OUTPUT);
                /*auto y =*/p1.Advance(0);
                expect(p1.GetState() == p1.S_OUTPUT);
                auto kind = p1.Advance(0);
                if (p1.GetState() == p1.S_OUTPUT && kind == 2)
                {
                    ++count;
                }
            }

            Printer::Print(__FILE__, "1", count);
        }

        {
            IntCode p2{prog};
            p2.SetMemory(0, 2);
            int joystick{0};

            int r{};
            int score{};
            int pad_x{} /*, pad_y{}*/;
            int ball_x{} /*, ball_y{}*/;

            while (p2.GetState() != p2.S_HALT)
            {
                switch (p2.GetState())
                {
                case p2.S_RUN:
                    r = p2.Advance(0);
                    break;
                case p2.S_OUTPUT:
                {
                    int x = r;
                    /*int y =*/ p2.Advance(0);
                    expect(p2.GetState() == p2.S_OUTPUT);
                    int z = p2.Advance(0);
                    expect(p2.GetState() == p2.S_OUTPUT);
                    if (x == -1)
                    {
                        score = z;
                    }
                    else if (z == 3)
                    {
                        pad_x = x;
                        //pad_y = y;
                    }
                    else if (z == 4)
                    {
                        ball_x = x;
                        //ball_y = y;
                    }
                    r = p2.Advance(0);
                }
                break;
                case p2.S_INPUT:
                    r = p2.Advance(joystick);
                    break;
                default:
                    break;
                }

                if (ball_x < pad_x)
                    joystick = -1;
                else if (ball_x > pad_x)
                    joystick = 1;
                else
                    joystick = 0;
            }

            Printer::Print(__FILE__, "2", score);
        }
    };
};

} //namespace;
