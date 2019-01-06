#include <doctest/doctest.h>
#include <fstream>
#include <sstream>
#include <vector>

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

    int FindMax() const
    {
        int m{};
        _FindMax(100, 0, {}, m);
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
            return res;
        }

        int GetScore() const
        {
            return std::max(capacity, 0) * std::max(durability, 0) * std::max(flavor, 0) * std::max(texture, 0);
        }
    };
    std::vector<Ingredient> _ingredients;

    void _FindMax(int spoons, size_t i, Ingredient acc, int &m) const
    {
        if (!spoons)
        {
            auto score = acc.GetScore();
            if (score > m)
                m = score;
            return;
        }

        if (i + 1 == _ingredients.size())
        {
            acc = acc.Add(spoons, _ingredients[i]);
            auto score = acc.GetScore();
            if (score > m)
                m = score;
            return;
        }

        for (int s = 0; s < spoons; ++s)
        {
            auto new_acc = acc.Add(s, _ingredients[i]);
            _FindMax(spoons - s, i + 1, new_acc, m);
        }
    }
};

} // namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        const char *const TEST =
            "Butterscotch: capacity -1, durability -2, flavor 6, texture 3, calories 8\n"
            "Cinnamon: capacity 2, durability 3, flavor -2, texture -1, calories 3\n";
        Recipe r(std::istringstream{TEST});
        REQUIRE(62842880 == r.FindMax());
    }

    SUBCASE("task") {
        Recipe r(std::ifstream{INPUT});
        MESSAGE(r.FindMax());
    }
}
