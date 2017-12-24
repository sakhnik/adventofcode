#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <fstream>

typedef std::pair<unsigned, unsigned> ComponentT;
typedef std::vector<ComponentT> InventoryT;

void Recurse(size_t chain_length, unsigned tip, InventoryT &inventory, unsigned accum, unsigned &max)
{
    for (size_t i = chain_length; i < inventory.size(); ++i)
    {
        auto [first, second] = inventory[i];
        if (first == tip)
        {
            std::swap(inventory[i], inventory[chain_length]);
            Recurse(chain_length + 1, second, inventory, accum + first + second, max);
            std::swap(inventory[i], inventory[chain_length]);
        }
        if (second == tip)
        {
            std::swap(inventory[i], inventory[chain_length]);
            Recurse(chain_length + 1, first, inventory, accum + first + second, max);
            std::swap(inventory[i], inventory[chain_length]);
        }
    }

    if (accum > max)
    {
        //for (size_t i = 0; i < chain_length; ++i)
        //    std::cout << " (" << inventory[i].first << "," << inventory[i].second << ")";
        //std::cout << " -> " << accum << std::endl;
        max = accum;
    }
}

unsigned Solve(InventoryT &inventory)
{
    unsigned result{0};
    Recurse(0, 0, inventory, 0, result);
    return result;
}

InventoryT Parse(std::istream &is)
{
    InventoryT inventory;
    unsigned a{0}, b{0};
    char skip;
    while (is >> a >> skip >> b)
        inventory.emplace_back(a, b);
    return inventory;
}

TEST_CASE("1")
{
    std::istringstream iss("0/2\n2/2\n2/3\n3/4\n3/5\n0/1\n10/1\n9/10");
    auto inventory = Parse(iss);
    REQUIRE(Solve(inventory) == 31);
}

TEST_CASE("main")
{
    std::ifstream ifs(INPUT);
    auto inventory = Parse(ifs);
    std::cout << Solve(inventory) << std::endl;
}
