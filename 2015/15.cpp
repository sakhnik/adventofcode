#include <fstream>
#include <sstream>
#include <vector>
#include "../test.hpp"
namespace {

class Recipe
{
public:
    Recipe(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            Ingredient i{};
            char skip[64];
            sscanf(line.c_str(),
                   "%s capacity %d, durability %d, flavor %d, texture %d, calories %d",
                   skip, &i.capacity, &i.durability, &i.flavor, &i.texture, &i.calories);
            _ingredients.push_back(i);
        }
    }

    int FindMax(int total_calories = -1) const
    {
        int m{};
        _FindMax(100, 0, {}, total_calories, m);
        return m;
    }

private:
    struct Ingredient
    {
        int capacity;
        int durability;
        int flavor;
        int texture;
        int calories;

        Ingredient Add(int spoons, const Ingredient &o) const
        {
            Ingredient res(*this);
            res.capacity += spoons * o.capacity;
            res.durability += spoons * o.durability;
            res.flavor += spoons * o.flavor;
            res.texture += spoons * o.texture;
            res.calories += spoons * o.calories;
            return res;
        }

        int GetScore() const
        {
            return std::max(capacity, 0) * std::max(durability, 0) * std::max(flavor, 0) * std::max(texture, 0);
        }
    };
    std::vector<Ingredient> _ingredients;

    void _FindMax(int spoons, size_t i, Ingredient acc, int total_calories, int &m) const
    {
        auto checkMax = [&]() {
            if (total_calories < 0 || acc.calories == total_calories)
            {
                auto score = acc.GetScore();
                if (score > m)
                    m = score;
            }
        };

        if (!spoons)
        {
            checkMax();
            return;
        }

        if (i + 1 == _ingredients.size())
        {
            acc = acc.Add(spoons, _ingredients[i]);
            checkMax();
            return;
        }

        for (int s = 0; s < spoons; ++s)
        {
            auto new_acc = acc.Add(s, _ingredients[i]);
            _FindMax(spoons - s, i + 1, new_acc, total_calories, m);
        }
    }
};

using namespace boost::ut;

suite s = [] {
    "15.test"_test = [] {
        const char *const TEST =
            "Butterscotch: capacity -1, durability -2, flavor 6, texture 3, calories 8\n"
            "Cinnamon: capacity 2, durability 3, flavor -2, texture -1, calories 3\n";
        Recipe r(std::istringstream{TEST});
        expect(62842880_i == r.FindMax());
        expect(57600000_i == r.FindMax(500));
    };

    "15.task"_test = [] {
        Recipe r(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", r.FindMax());
        Printer::Print(__FILE__, "2", r.FindMax(500));
    };
};

} // namespace;
