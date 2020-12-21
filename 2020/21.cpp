#include "../test.hpp"
#include <set>
#include <map>

namespace {

size_t CountMenu(std::istream &&is)
{
    using SetT = std::set<std::string>;
    std::map<std::string, SetT> allergen2ingredients;
    std::vector<std::string> all_ingredients;

    std::string line;
    while (getline(is, line))
    {
        const char *const SPLIT = "(contains ";
        auto i = line.find(SPLIT);
        std::string_view si(line.data(), i);
        std::string_view sa(line.data() + i + strlen(SPLIT));

        SetT ingredients;

        for (auto i = si.find(' ');
             i != si.npos;
             si = si.substr(i + 1), i = si.find(' '))
        {
            std::string ingr{si.substr(0, i)};
            ingredients.insert(ingr);
            all_ingredients.push_back(ingr);
        }

        while (true)
        {
            auto i = sa.find_first_of(", )");
            if (i == sa.npos)
                break;

            std::string allergen{sa.substr(0, i)};
            auto it = allergen2ingredients.find(allergen);
            if (it == allergen2ingredients.end())
                allergen2ingredients[allergen] = ingredients;
            else
            {
                SetT common;
                std::set_intersection(it->second.begin(), it->second.end(),
                                      ingredients.begin(), ingredients.end(),
                                      std::inserter(common, common.end()));
                it->second.swap(common);
            }

            sa = sa.substr(i);
            i = sa.find_first_not_of(", )");
            if (i == sa.npos)
                break;
            sa = sa.substr(i);
        }
    }

    std::map<std::string, std::string> ingredient2allergen;

    while (!allergen2ingredients.empty())
    {
        auto it = std::find_if(allergen2ingredients.begin(), allergen2ingredients.end(),
                               [](const auto &ai) {
                                    return ai.second.size() == 1;
                               });
        if (it != allergen2ingredients.end())
        {
            std::string ingr{*it->second.begin()};
            ingredient2allergen[ingr] = it->first;
            for (auto &ai : allergen2ingredients)
                ai.second.erase(ingr);
            allergen2ingredients.erase(it);
        }
        else
            throw "No solution";
    }

    return std::count_if(all_ingredients.begin(), all_ingredients.end(),
                         [&](const std::string &ingr) {
                            return !ingredient2allergen.contains(ingr);
                         });
}


using namespace boost::ut;

suite s = [] {
    "2020-21"_test = [] {
        {
            const char *const TEST = R"(mxmxvkd kfcds sqjhc nhms (contains dairy, fish)
trh fvjkl sbzzf mxmxvkd (contains dairy)
sqjhc fvjkl (contains soy)
sqjhc mxmxvkd sbzzf (contains fish))";
            expect(5_u == CountMenu(std::istringstream{TEST}));
        }

        Printer::Print(__FILE__, "1", CountMenu(std::ifstream{INPUT}));
    };
};

} //namespace;
