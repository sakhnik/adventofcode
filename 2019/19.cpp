#include "IntCode.h"
#include <fstream>
#include "../test.hpp"

namespace {

using namespace boost::ut;

bool TestLocation(int x, int y, IntCode prog)
{
    auto r = prog.Advance(0);
    expect(prog.GetState() == prog.S_INPUT);
    r = prog.Advance(x);
    expect(prog.GetState() == prog.S_INPUT);
    r = prog.Advance(y);
    expect(prog.GetState() == prog.S_OUTPUT);
    prog.Advance(0);
    expect(prog.GetState() == prog.S_HALT);
    return r == 1;
}

int CountArea(int x0, int y0, int x_max, int y_max, const IntCode &prog)
{
    int x{x0}, y{y0};
    int count{1};
    int x_min{x0};

    bool isTracking{true};  // x0,y0 is known to be the beam

    auto nextRow = [&]() {
        isTracking = false;
        x = x_min;
        return ++y < y_max;
    };


    while (true)
    {
        if (TestLocation(x, y, prog))
        {
            isTracking = true;
            ++count;
            if (++x >= x_max)
            {
                if (!nextRow())
                    break;
            }
        }
        else
        {
            if (!isTracking)
            {
                x_min = ++x;
                if (x_min >= x_max)
                    break;
            }
            else
            {
                if (!nextRow())
                    break;
            }
        }
    }

    return count;
}

int FindFit(int x0, int y0, int size, const IntCode &prog)
{
    int x{x0}, y{y0};
    int x_min{x0};


    while (true)
    {
        if (TestLocation(x, y, prog))
        {
            // (x,y) may be the bottom left corner
            // Now check the top right corner
            int x1 = x + size - 1;
            int y1 = y - size + 1;

            if (y1 > 0 && TestLocation(x1, y1, prog))
            {
                return x * 10000 + y1;
            }

            x = x_min;
            ++y;
        }
        else
        {
            x_min = ++x;
        }
    }
}

suite s = [] {
    "19"_test = [] {
        std::ifstream ifs{INPUT};
        IntCode prog{ifs};

        for (int y = 0; y < 20; ++y)
        {
            for (int x = 0; x < 20; ++x)
            {
                std::cout << TestLocation(x, y, prog);
            }
            std::cout << std::endl;
        }

        Printer::Print(__FILE__, "1", CountArea(3, 4, 50, 50, prog));
        Printer::Print(__FILE__, "2", FindFit(3, 4, 100, prog));
    };
};

} //namespace;
