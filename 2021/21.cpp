#include "../test.hpp"

namespace {

class DiracDice
{
public:
    int rolls[2] = {};
    int points[2] = {};
    int dice{};
    int player = 0;
    size_t roll_count{};

    DiracDice(std::array<int, 2> position)
    {
        while (true)
        {
            int roll = Roll() + Roll() + Roll();
            position[player] = (position[player] - 1 + roll) % 10 + 1;
            points[player] += position[player];
            if (points[player] >= 1000)
                break;
            player = 1 - player;
        }
    }

    int Roll()
    {
        ++roll_count;
        if (++dice <= 100)
            return dice;
        return dice = 1;
    }

    size_t GetScore() const
    {
        return roll_count * points[1 - player];
    }
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-21"_test = [] {
        DiracDice test({4, 8});
        expect(739785_u == test.GetScore());

        DiracDice dice({1, 6});
        Printer::Print(__FILE__, "1", dice.GetScore());
    };
};

} //namespace;
