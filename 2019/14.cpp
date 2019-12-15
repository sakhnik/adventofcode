#include <doctest/doctest.h>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <unordered_set>


namespace {

struct Ingredient
{
    int amount;
    std::string name;
};

struct Reaction
{
    int outcome;
    std::vector<Ingredient> ingredients;
};

using ReactionsT = std::map<std::string, Reaction>;

ReactionsT ParseInput(std::istream &&is)
{
    ReactionsT ret;

    std::string line;
    while (getline(is, line))
    {
        std::string name;
        Reaction r;

        auto i = line.find(" => ");
        std::istringstream iss1(line.substr(i + 4));
        iss1 >> r.outcome >> name;

        std::istringstream iss2(line.substr(0, i));
        Ingredient ingr;
        auto &chem = ingr.name;
        while (iss2 && (iss2 >> ingr.amount >> chem))
        {
            if (chem.back() == ',')
                chem.erase(chem.size() - 1);
            r.ingredients.push_back(ingr);
        }

        ret.emplace(std::move(name), std::move(r));
    }

    return ret;
}

std::vector<std::string> TopoSort(const std::string &name, const ReactionsT &reactions)
{
    std::vector<std::string> result;
    std::unordered_set<std::string> visited;

    auto impl = [&result, &visited, &reactions](auto &name, auto &impl) {
        if (name == "ORE")
            return;
        visited.insert(name);
        const auto &[amount, requirements] = reactions.at(name);
        for (const auto &[quant, ingr] : requirements)
        {
            if (!visited.count(ingr))
                impl(ingr, impl);
        }
        result.push_back(name);
    };

    impl(name, impl);
    return result;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
    auto reactions = ParseInput(std::ifstream{INPUT});

    auto order = TopoSort("FUEL", reactions);

    auto oreNeeded = [&](int fuel) {
        std::map<std::string, int> amountNeeded{{"FUEL", fuel}};
        for (auto it = rbegin(order); it != rend(order); ++it)
        {
            const auto &[outcome, ingredients] = reactions[*it];
            for (const auto &[amount, name] : ingredients)
            {
                auto reactionCount = (amountNeeded[*it] + outcome - 1) / outcome;
                amountNeeded[name] += amount * reactionCount;
            }
        }
        return amountNeeded["ORE"];
    };

    MESSAGE(oreNeeded(1));
}
