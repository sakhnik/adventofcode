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
    {0, 0, 0},
};

const Item ARMOR[] = {
    {13, 0, 1},
    {31, 0, 2},
    {53, 0, 3},
    {75, 0, 4},
    {102, 0, 5},
    {0, 0, 0},
};

const Item RINGS[] = {
    {25, 1, 0},
    {50, 2, 0},
    {100, 3, 0},
    {20, 0, 1},
    {40, 0, 2},
    {80, 0, 3},
    {0, 0, 0},
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

int FindMinimumCost()
{
    int minCost = std::numeric_limits<int>::max();

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
                    if (ri1 != 0 && ri1 == ri2)
                    {
                        continue;
                    }
                    Item player = weaponArmorRing1 + RINGS[ri2];
                    int cost = player.cost;
                    player.cost = PLAYER_HP;
                    if (!PlayerWins(player, BOSS))
                    {
                        continue;
                    }

                    if (cost < minCost)
                    {
                        minCost = cost;
                    }
                }
            }
        }
    }

    return minCost;
}

using namespace boost::ut;

suite s = [] {
    "2015-21"_test = [] {
        expect(PlayerWins({8, 5, 5}, {12, 7, 2}));

        Printer::Print(__FILE__, "1", FindMinimumCost());
    };
};

} //namespace;