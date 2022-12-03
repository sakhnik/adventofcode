#include "../test.hpp"
#include <fstream>

namespace {

const int MATCH[3][3] = {
    // Rock, Paper, Scissors
    {  1,    0,     2 },  // Rock
    {  2,    1,     0 },  // Paper
    {  0,    2,     1 }  // Scissors
};

const int VALUE[3] = { 1, 2, 3 };

const char *const TEST = "A Y\nB X\nC Z";

int CalcScore(std::istream &&is)
{
    int score{};

    char a{}, b{};
    while (is >> a >> b)
    {
        int opponent = a - 'A';
        int player = b - 'X';
        score += 3 * MATCH[player][opponent] + VALUE[player];
    }
    return score;
}

using namespace boost::ut;

suite s = [] {
    "2022-02"_test = [] {
        expect(15_i == CalcScore(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", CalcScore(std::ifstream{INPUT}));
        //Printer::Print(__FILE__, "2", res.second);
    };
};

} //namespace;
