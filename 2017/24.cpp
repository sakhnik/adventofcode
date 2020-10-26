#include <fstream>
#include <vector>
#include <sstream>
#include "../test.hpp"

namespace {

typedef std::pair<unsigned, unsigned> ComponentT;
typedef std::vector<ComponentT> InventoryT;

struct Objective
{
    unsigned max_absolute = 0;   // max value among all the sequences
    unsigned max_longest = 0;    // max value among the longest sequences
    size_t longest = 0;          // the length of the longest sequence
};

// We'll be reordering sequences so that the valid sequences will appear
// at the beginning of the inventory. We'll be restoring the initial order
// upon return from the recursive probe to ensure all the variants are considered.
//
//  [+++........]
//
// Thus, at each stage we'll have to search for a next candidate in the right half.
void Recurse(size_t chain_length, unsigned tip, InventoryT &inventory, unsigned accum, Objective &objective)
{
    for (size_t i = chain_length; i < inventory.size(); ++i)
    {
        auto [first, second] = inventory[i];
        if (first == tip)
        {
            std::swap(inventory[i], inventory[chain_length]);
            Recurse(chain_length + 1, second, inventory, accum + first + second, objective);
            std::swap(inventory[i], inventory[chain_length]);
        }
        if (second == tip)
        {
            std::swap(inventory[i], inventory[chain_length]);
            Recurse(chain_length + 1, first, inventory, accum + first + second, objective);
            std::swap(inventory[i], inventory[chain_length]);
        }
    }

    if (accum > objective.max_absolute)
    {
        //for (size_t i = 0; i < chain_length; ++i)
        //    std::cout << " (" << inventory[i].first << "," << inventory[i].second << ")";
        //MESSAGE(" -> " << accum);
        objective.max_absolute = accum;
    }

    if (chain_length > objective.longest)
    {
        objective.longest = chain_length;
        objective.max_longest = accum;
    }
    else if (chain_length == objective.longest)
    {
        if (objective.max_longest < accum)
            objective.max_longest = accum;
    }
}

Objective Solve(InventoryT &inventory)
{
    Objective result;
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

using namespace boost::ut;

suite s = [] {
    "2017-24"_test = [] {
        {
            std::istringstream iss("0/2\n2/2\n2/3\n3/4\n3/5\n0/1\n10/1\n9/10");
            auto inventory = Parse(iss);
            auto result = Solve(inventory);
            expect(result.max_absolute == 31_u);
            expect(result.max_longest == 19_u);
        }

        std::ifstream ifs(INPUT);
        auto inventory = Parse(ifs);
        auto result = Solve(inventory);
        Printer::Print(__FILE__, "1", result.max_absolute);
        Printer::Print(__FILE__, "2", result.max_longest);
    };
};

} //namespace;