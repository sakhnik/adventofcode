#include "../test.hpp"

namespace {

struct Item
{
    int cost;
    int damage;
    int armor;
};

Item operator+(const Item &a, const Item &b)
{
    return {a.cost + b.cost, a.damage + b.damage, a.armor + b.armor};
}

const Item WEAPONS[] = {
    {8, 4, 0},
    {10, 5, 0},
    {25, 6, 0},
    {40, 7, 0},
    {74, 8, 0},
};

const Item ARMOR[] = {
    {0, 0, 0},
    {13, 0, 1},
    {31, 0, 2},
    {53, 0, 3},
    {75, 0, 4},
    {102, 0, 5},
};

const Item RINGS[] = {
    {0, 0, 0},
    {25, 1, 0},
    {50, 2, 0},
    {100, 3, 0},
    {20, 0, 1},
    {40, 0, 2},
    {80, 0, 3},
};

const int PLAYER_HP = 100;
const Item BOSS = { 103, 9, 2 };

bool PlayerWins(const Item &player, const Item &boss)
{
    int playerDeals = player.damage - boss.armor;
    if (playerDeals <= 0)
    {
        playerDeals = 1;
    }
    int playerMoves = (boss.cost + playerDeals - 1) / playerDeals;

    int bossDeals = boss.damage - player.armor;
    if (bossDeals <= 0)
    {
        bossDeals = 1;
    }
    int bossMoves = (player.cost + bossDeals - 1) / bossDeals;

    return playerMoves <= bossMoves;
};

template <typename TrackerT>
int FindCost()
{
    TrackerT tracker;

    for (size_t wi = 0, wN = std::size(WEAPONS); wi < wN; ++wi)
    {
        const Item &weapon = WEAPONS[wi];
        for (size_t ai = 0, aN = std::size(ARMOR); ai < aN; ++ai)
        {
            Item weaponArmor = weapon + ARMOR[ai];
            for (size_t ri1 = 0, rN = std::size(RINGS); ri1 < rN; ++ri1)
            {
                Item weaponArmorRing1 = weaponArmor + RINGS[ri1];
                for (size_t ri2 = 0; ri2 < rN; ++ri2)
                {
                    if (ri1 != 0 && ri2 != 0 && ri1 == ri2)
                    {
                        continue;
                    }
                    Item player = weaponArmorRing1 + RINGS[ri2];
                    tracker.Check(player);
                }
            }
        }
    }

    return tracker.Result();
}

class MinCostTracker
{
private:
    int minCost = std::numeric_limits<int>::max();

public:
    void Check(Item player)
    {
        int cost = player.cost;
        player.cost = PLAYER_HP;
        if (PlayerWins(player, BOSS) && cost < minCost)
        {
            minCost = cost;
        }
    }

    int Result() const { return minCost; }
};


class MaxCostTracker
{
private:
    int maxCost = std::numeric_limits<int>::min();

public:
    void Check(Item player)
    {
        int cost = player.cost;
        player.cost = PLAYER_HP;
        if (!PlayerWins(player, BOSS) && cost > maxCost)
        {
            maxCost = cost;
        }
    }

    int Result() const { return maxCost; }
};

using namespace boost::ut;

suite s = [] {
    "2015-21"_test = [] {
        expect(PlayerWins({8, 5, 5}, {12, 7, 2}));

        Printer::Print(__FILE__, "1", FindCost<MinCostTracker>());
        Printer::Print(__FILE__, "2", FindCost<MaxCostTracker>());
    };
};

} //namespace;