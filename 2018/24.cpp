#include <doctest/doctest.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <regex>
#include <iostream>
#include <unordered_set>
#include <numeric>
#include <algorithm>
#include <cassert>

namespace {

class Fight
{
public:
    Fight(std::istream &&is)
    {
        _Side *side{};

        std::string line;
        while (is && getline(is, line))
        {
            if (line.empty())
                continue;
            if (line.back() == ':')
            {
                side = &_sides.emplace_back();
                continue;
            }

            static const std::regex r{R"((\d+) units each with (\d+) hit points (\([^)]+\))? ?with an attack that does (\d+) (\w+) damage at initiative (\d+))"};
            std::smatch m;
            REQUIRE(std::regex_match(line, m, r));
            _Group group{
                .count = std::stoi(m[1]),
                .hp = std::stoi(m[2]),
                .damage = std::stoi(m[4]),
                .attack_type = m[5],
                .initiative = std::stoi(m[6]),
            };

            const static std::regex r2{R"((\w+) to ([^;]+))"};
            for (std::regex_token_iterator<std::string::const_iterator>
                     rend{},
                     rit(m[3].first, m[3].second, r2, {1,2});
                rit != rend;
                )
            {
                const auto &kind = (rit++)->str();
                const auto &attacks = (rit++);

                auto &t = kind == "immune" ? group.immunity : group.weakness;

                const static std::regex r3{R"((\w+))"};
                for (std::regex_iterator<std::string::const_iterator>
                         itend{},
                         it(attacks->first, attacks->second, r3);
                     it != itend; ++it)
                {
                    t.emplace(it->str());
                }
            }

            side->emplace_back(std::move(group));
        }

        REQUIRE(2 == _sides.size());
    }

    void Print()
    {
        for (const auto &s : _sides)
        {
            for (const auto &u : s)
            {
                std::cout << u.count
                    << " " << u.hp
                    << " " << u.damage
                    << " " << u.attack_type
                    << " " << u.initiative << " ";
                std::cout << "i:";
                for (const auto &i : u.immunity)
                    std::cout << i << ",";
                std::cout << " w:";
                for (const auto &i : u.weakness)
                    std::cout << i << ",";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }

    void Round()
    {
        using TargetsT = std::vector<size_t>;
        std::vector<TargetsT> targetsForSides;

        for (size_t i = 0; i < _sides.size(); ++i)
        {
            auto &attackers = _sides[i];

            std::sort(attackers.begin(), attackers.end(),
                [](const auto &a, const auto &b) {
                        auto pa = a.GetEffectivePower();
                        auto pb = b.GetEffectivePower();
                        if (pa > pb) return true;
                        if (pa < pb) return false;
                        return a.initiative > b.initiative;
                 });
        }

        for (size_t i = 0; i < _sides.size(); ++i)
        {
            auto &attackers = _sides[i];
            auto &defenders = _sides[1 - i];

            auto &targets = targetsForSides.emplace_back(defenders.size());
            std::iota(targets.begin(), targets.end(), 0);

            auto defenderIt = targets.begin();
            auto attackerIt = attackers.begin();

            for (; defenderIt != targets.end() && attackerIt != attackers.end();
                 ++defenderIt, ++attackerIt)
            {
                const auto &attacker = *attackerIt;
                auto it = std::max_element(defenderIt, targets.end(),
                                           [&attacker, &defenders](auto a, auto b)
                                           {
                                                const auto &da = defenders[a];
                                                const auto &db = defenders[b];
                                                auto damA = da.GetDamage(attacker);
                                                auto damB = db.GetDamage(attacker);
                                                if (damA < damB) return true;
                                                if (damA > damB) return false;
                                                auto pa = da.GetEffectivePower();
                                                auto pb = db.GetEffectivePower();
                                                if (pa < pb) return true;
                                                if (pa > pb) return false;
                                                return da.initiative < db.initiative;
                                           });
                if (defenders[*it].GetDamage(attacker) <= 0)
                {
                    defenderIt = targets.insert(defenderIt, defenders.size());
                }
                else
                {
                    std::swap(*it, *defenderIt);
                }
            }

            targets.erase(defenderIt, targets.end());
            while (targets.size() < attackers.size())
                targets.push_back(defenders.size());
            assert(targets.size() == attackers.size());
        }

        struct Idx
        {
            size_t side;
            size_t idx;
        };
        std::vector<Idx> idx;

        for (size_t i = 0; i < _sides.size(); ++i)
        {
            for (size_t j = 0; j < _sides[i].size(); ++j)
            {
                idx.push_back({i, j});
            }
        }

        std::sort(begin(idx), end(idx),
             [this](auto a, auto b) {
                 return _sides[a.side][a.idx].initiative > _sides[b.side][b.idx].initiative;
             });

        for (auto i : idx)
        {
            assert(i.side < _sides.size());
            const auto &allies = _sides[i.side];
            const auto &targets = targetsForSides[i.side];
            assert(allies.size() == targets.size());
            auto &enemies = _sides[1 - i.side];

            assert(i.idx < allies.size());
            const auto &attacker = allies[i.idx];
            auto target = targets[i.idx];
            if (target >= enemies.size())
                continue;

            auto &defender = enemies[target];
            if (defender.count <= 0)
                continue;

            auto damage = defender.GetDamage(attacker);
            defender.count -= damage / defender.hp;

            //std::cout << i.side << "." << i.idx << " attacks " << (1-i.side) << "." << target
            //    << ", deals " << damage << " damage => kills " << (damage / defender.hp)
            //    << std::endl;
        }

        for (auto &side : _sides)
        {
            auto it = std::remove_if(side.begin(), side.end(),
                [](const auto &g) {
                     return g.count <= 0;
                });
            side.erase(it, side.end());
        }
    }

    int RunUntilVictory()
    {
        while (!_sides[0].empty() && !_sides[1].empty())
            Round();
        auto &victor = _sides[0].empty() ? _sides[1] : _sides[0];
        return std::accumulate(victor.begin(), victor.end(), 0,
                               [](int a, const auto &g) {
                                    return a + g.count;
                               });
    }

private:
    struct _Group
    {
        int count;
        int hp;
        int damage;
        std::string attack_type;
        int initiative;
        std::unordered_set<std::string> immunity, weakness;

        _Group(const _Group &o) = delete;
        _Group& operator=(const _Group &o) = delete;
        _Group(_Group &&o) = default;
        _Group& operator=(_Group &&) = default;

        int GetEffectivePower() const
        {
            return count * damage;
        }

        int GetDamage(const _Group &o) const
        {
            if (immunity.count(o.attack_type))
                return 0;
            int dam = o.GetEffectivePower();
            if (weakness.count(o.attack_type))
                dam *= 2;
            return dam;
        }
    };
    using _Side = std::vector<_Group>;
    std::vector<_Side> _sides;
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        Fight f(std::istringstream{R"(Immune System:
17 units each with 5390 hit points (weak to radiation, bludgeoning) with an attack that does 4507 fire damage at initiative 2
989 units each with 1274 hit points (immune to fire; weak to bludgeoning, slashing) with an attack that does 25 slashing damage at initiative 3

Infection:
801 units each with 4706 hit points (weak to radiation) with an attack that does 116 bludgeoning damage at initiative 1
4485 units each with 2961 hit points (immune to radiation; weak to fire, cold) with an attack that does 12 slashing damage at initiative 4
)"});
        f.Round();
        REQUIRE(5216 == f.RunUntilVictory());
    }

    SUBCASE("task") {
        Fight f(std::ifstream{INPUT});
        MESSAGE(f.RunUntilVictory());
    }
}
