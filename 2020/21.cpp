#include "../test.hpp"
#include <set>
#include <map>

namespace {

class Menu
{
public:
    Menu(std::istream &&is)
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

        while (!allergen2ingredients.empty())
        {
            auto it = std::find_if(allergen2ingredients.begin(), allergen2ingredients.end(),
                                   [](const auto &ai) {
                                        return ai.second.size() == 1;
                                   });
            if (it != allergen2ingredients.end())
            {
                std::string ingr{*it->second.begin()};
                _ingredient2allergen[ingr] = it->first;
                for (auto &ai : allergen2ingredients)
                    ai.second.erase(ingr);
                allergen2ingredients.erase(it);
            }
            else
                throw "No solution";
        }

        _inert_count =
            std::count_if(all_ingredients.begin(), all_ingredients.end(),
                          [this](const std::string &ingr) {
                              return !_ingredient2allergen.contains(ingr);
                          });
    }

    size_t GetInertCount() const { return _inert_count; }

    std::string GetDanger() const
    {
        std::map<std::string, std::string> allergen2ingredient;
        for (auto &ia : _ingredient2allergen)
        {
            allergen2ingredient[ia.second] = ia.first;
        }
        std::string list;
        for (const auto &ai : allergen2ingredient)
        {
            if (!list.empty())
                list += ",";
            list += ai.second;
        }
        return list;
    }

private:
    std::map<std::string, std::string> _ingredient2allergen;
    size_t _inert_count = 0;
};


using namespace boost::ut;

suite s = [] {
    "2020-21"_test = [] {
        {
            const char *const TEST = R"(mxmxvkd kfcds sqjhc nhms (contains dairy, fish)
trh fvjkl sbzzf mxmxvkd (contains dairy)
sqjhc fvjkl (contains soy)
sqjhc mxmxvkd sbzzf (contains fish))";
            Menu m{std::istringstream{TEST}};
            expect(5_u == m.GetInertCount());
            expect(m.GetDanger() == "mxmxvkd,sqjhc,fvjkl");
        }

        Menu m{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", m.GetInertCount());
        Printer::Print(__FILE__, "2", m.GetDanger());
    };
};

} //namespace;
