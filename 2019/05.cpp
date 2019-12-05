#include <doctest/doctest.h>
#include <fstream>
#include <sstream>

#include "IntCode.h"


TEST_CASE(TEST_NAME)
{
    std::ifstream ifs{INPUT};
    IntCode master{ifs};

    std::istringstream iss{"1"};
    std::ostringstream oss;
    master.Run(iss, oss);
    MESSAGE(oss.str());
}
