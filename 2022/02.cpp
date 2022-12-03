#include "../test.hpp"
#include <fstream>

namespace {

const int MATCH[3][3] = {
    // Rock, Paper, Scissors
    {  1,    0,     2 },  // Rock
    {  2,    1,     0 },  // Paper
    {  0,    2,     1 }   // Scissors
};

const int MATCH2[3][3] = {
    // Rock, Paper, Scissors
    {  2,    0,     1 },  // Lose
    {  0,    1,     2 },  // Draw
    {  1,    2,     0 }   // Win
};

const int VALUE[3] = { 1, 2, 3 };

const char *const TEST = "A Y\nB X\nC Z";

std::pair<int, int> CalcScore(std::istream &&is)
{
    int score1{}, score2{};

    char a{}, b{};
    while (is >> a >> b)
    {
        int opponent = a - 'A';
        int player = b - 'X';

        // strategy 1
        score1 += 3 * MATCH[player][opponent] + VALUE[player];

        // strategy 2
        int choice = MATCH2[player][opponent];
        score2 += 3 * player + VALUE[choice];
    }
    return {score1, score2};
}

using namespace boost::ut;

suite s = [] {
    "2022-02"_test = [] {
        auto test_score = CalcScore(std::istringstream{TEST});
        expect(15_i == test_score.first);
        expect(12_i == test_score.second);

        auto score = CalcScore(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", score.first);
        Printer::Print(__FILE__, "2", score.second);
    };
};

} //namespace;
