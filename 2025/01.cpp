#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>

namespace {

struct Dial
{
    int count1 = 0;
    int count2 = 0;
    int pos = 50;

    Dial(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line)) {
            int sign = line[0] == 'L' ? -1 : 1;
            int clicks = std::stoi(line.substr(1));

            auto nextPos = pos + (clicks * sign);
            int start = std::min(pos, nextPos);
            int finish = std::max(pos, nextPos);
            count2 += std::floor(finish / 100.0) - std::floor((start - 1) / 100.0) - (pos == 0);

            pos = ((nextPos % 100) + 100) % 100;
            count1 += pos == 0;
        }
    }
};

const char *const TEST1 = R"(L68
L30
R48
L5
R60
L55
L1
L99
R14
L82)";

} //namespace;
 
int main()
{
    Dial test{std::istringstream{TEST1}};
    assert(test.count1 == 3);
    assert(test.count2 == 6);

    Dial run{std::ifstream{"01.txt"}};
    std::cout << run.count1 << std::endl;
    std::cout << run.count2 << std::endl;
    return 0;
}
