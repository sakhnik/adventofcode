#include "../test.hpp"

namespace {

enum class Winner
{
    UNKNOWN = 0,
    PLAYER = 1,
    BOSS = 2,
};

struct State
{
    int playerHp = 50;
    int playerMana = 500;
    int playerArmor = 0;
    int bossHp = 71;
    int bossDamage = 10;
    int shieldTimeout = 0;
    int poisonTimeout = 0;
    int rechargeTimeout = 0;
    int manaSpent = 0;

    bool PlayerWins() const
    {
        return bossHp <= 0;
    }

    bool BossWins() const
    {
        return playerMana <= 0 || playerHp <= 0;
    }
};

State ApplyEffects(const State &s)
{
    State res{s};
    if (res.shieldTimeout)
    {
        // Shield costs 113 mana. It starts an effect that lasts for 6 turns.
        // While it is active, your armor is increased by 7.
        if (!res.playerArmor)
            res.playerArmor = 7;
        if (!--res.shieldTimeout)
            res.playerArmor = 0;
    }
    if (res.poisonTimeout)
    {
        // Poison costs 173 mana. It starts an effect that lasts for 6 turns.
        // At the start of each turn while it is active, it deals the boss 3 damage.
        res.bossHp -= 3;
        --res.poisonTimeout;
    }
    if (res.rechargeTimeout)
    {
        // Recharge costs 229 mana. It starts an effect that lasts for 5 turns.
        // At the start of each turn while it is active, it gives you 101 new mana.
        res.playerMana += 101;
        --res.rechargeTimeout;
    }
    return res;
}

State MoveBoss(const State &s)
{
    State res = s;
    int attack = res.bossDamage - res.playerArmor;
    if (attack < 0)
        attack = 1;
    res.playerHp -= attack;
    return res;
}

State TryMissile(const State &s)
{
    // Magic Missile costs 53 mana. It instantly does 4 damage.
    State res = s;
    res.manaSpent += 53;
    res.playerMana -= 53;
    res.bossHp -= 4;
    return res;
}

State TryDrain(const State &s)
{
    // Drain costs 73 mana. It instantly does 2 damage and heals you for 2 hit points.
    State res = s;
    res.manaSpent += 73;
    res.playerMana -= 73;
    res.bossHp -= 2;
    res.playerHp += 2;
    return res;
}

State TryShield(const State &s)
{
    State res = s;
    res.manaSpent += 113;
    res.playerMana -= 113;
    res.shieldTimeout = 6;
    return res;
}

State TryPoison(const State &s)
{
    State res = s;
    res.manaSpent += 173;
    res.playerMana -= 173;
    res.poisonTimeout = 6;
    return res;
}

State TryRecharge(const State &s)
{
    State res = s;
    res.manaSpent += 229;
    res.playerMana -= 229;
    res.rechargeTimeout = 5;
    return res;
}

struct Accumulator
{
    int minManaSpent = std::numeric_limits<int>::max();

    bool Check(const State &s)
    {
        // Prune detected non-optimal branches.
        if (s.manaSpent >= minManaSpent)
            return true;

        if (s.BossWins())
            return true;
        if (s.PlayerWins())
        {
            if (s.manaSpent < minManaSpent)
                minManaSpent = s.manaSpent;
            return true;
        }
        return false;
    }
};

void Explore(const State &s, Accumulator &acc)
{
    auto proceed = [&acc](State curState) {
        if (acc.Check(curState))
            return;
        curState = ApplyEffects(curState);
        if (acc.Check(curState))
            return;
        curState = MoveBoss(curState);
        if (acc.Check(curState))
            return;
        curState = ApplyEffects(curState);
        if (acc.Check(curState))
            return;
        Explore(curState, acc);
    };

    proceed(TryMissile(s));
    proceed(TryDrain(s));
    if (!s.poisonTimeout)
        proceed(TryPoison(s));
    if (!s.shieldTimeout)
        proceed(TryShield(s));
    if (!s.rechargeTimeout)
        proceed(TryRecharge(s));
}

using namespace boost::ut;

suite s = [] {
    "2015-22"_test = [] {
        Accumulator acc;
        Explore(State{}, acc);
        Printer::Print(__FILE__, "1", acc.minManaSpent);
    };
};

} //namespace;