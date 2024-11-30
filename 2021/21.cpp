#include "../test.hpp"
#include <unordered_map>

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

class DiracDice2
{
public:
    static const int GOAL_SCORE = 21;
    using WinsT = std::pair<size_t, size_t>;
    using StateMapT = std::unordered_map<uint32_t, WinsT>;
    StateMapT state_map;

    WinsT wins = {};

    DiracDice2(int pos1, int pos2)
        : wins{CountWins(pos1, 0, pos2, 0)}
    {
    }

    static uint32_t GetStateKey(int pl_pos, int pl_score, int op_pos, int op_score)
    {
        return (pl_pos << 24) | (pl_score << 16) | (op_pos << 8) | op_score;
    }

    WinsT CountWins(int8_t pl_pos, int8_t pl_score, int8_t op_pos, int8_t op_score)
    {
        if (pl_score >= GOAL_SCORE)
            return {1, 0};
        if (op_score >= GOAL_SCORE)
            return {0, 1};

        auto state_key = GetStateKey(pl_pos, pl_score, op_pos, op_score);
        auto it = state_map.find(state_key);
        if (it != state_map.end())
            return it->second;

        size_t pl_wins{}, op_wins{};
        for (int d1 = 1; d1 <= 3; ++d1)
            for (int d2 = 1; d2 <= 3; ++d2)
                for (int d3 = 1; d3 <= 3; ++d3)
                {
                    int new_pos = (pl_pos + d1 + d2 + d3 - 1) % 10 + 1;
                    int new_score = pl_score + new_pos;
                    auto [ow, pw] = CountWins(op_pos, op_score, new_pos, new_score);
                    op_wins += ow;
                    pl_wins += pw;
                }
        state_map[state_key] = {pl_wins, op_wins};
        return {pl_wins, op_wins};
    }

    size_t GetWinnerCounts() const
    {
        return std::max(wins.first, wins.second);
    }
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "21"_test = [] {
        expect(739785_u == DiracDice({4, 8}).GetScore());
        expect(eq(444356092776315ull, DiracDice2(4, 8).GetWinnerCounts()));

        DiracDice dice({1, 6});
        Printer::Print(__FILE__, "1", dice.GetScore());
        Printer::Print(__FILE__, "2", DiracDice2(1, 6).GetWinnerCounts());
    };
};

} //namespace;
