#include <doctest/doctest.h>
#include "IntCode.h"
#include <fstream>


bool TestLocation(int x, int y, IntCode prog)
{
    auto r = prog.Advance(0);
    REQUIRE(prog.GetState() == prog.S_INPUT);
    r = prog.Advance(x);
    REQUIRE(prog.GetState() == prog.S_INPUT);
    r = prog.Advance(y);
    REQUIRE(prog.GetState() == prog.S_OUTPUT);
    prog.Advance(0);
    REQUIRE(prog.GetState() == prog.S_HALT);
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

TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCode prog{ifs};

    for (int y = 0; y < 10; ++y)
    {
        for (int x = 0; x < 10; ++x)
        {
            std::cout << TestLocation(x, y, prog);
        }
        std::cout << std::endl;
    }

    MESSAGE(CountArea(3, 4, 50, 50, prog));
}
