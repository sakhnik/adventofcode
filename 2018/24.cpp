#include <sstream>
#include <fstream>
#include <vector>
#include <regex>
#include <unordered_set>
#include <numeric>
#include <algorithm>
#include <cassert>
#include "../test.hpp"

namespace {

using namespace boost::ut;

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
            expect(std::regex_match(line, m, r));
            _Group group{
                .id = side->size() + 1,
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

        expect(2_u == _sides.size());
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

    void Boost(size_t side, int value)
    {
        for (auto &group : _sides[side])
        {
            group.damage += value;
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
            if (defender.count <= 0 || attacker.count <= 0)
                continue;

            auto damage = defender.GetDamage(attacker);
            defender.count -= damage / defender.hp;

            //std::cout
            //    << "initiative " << attacker.initiative << ": "
            //    << (i.side + 1) << "." << attacker.id
            //    << " attacks " << (2-i.side) << "." << defender.id
            //    << ", deals " << damage << " damage => kills " << (damage / defender.hp)
            //    << "\n";
        }
        //std::cout << std::endl;

        for (auto &side : _sides)
        {
            auto it = std::remove_if(side.begin(), side.end(),
                [](const auto &g) {
                     return g.count <= 0;
                });
            side.erase(it, side.end());
        }
    }

    size_t RunUntilVictory()
    {
        std::vector<size_t> prev_counts(_sides.size()), counts(_sides.size());
        for (size_t i = 0; i < _sides.size(); ++i)
            prev_counts[i] = GetUnitCount(i);

        while (true)
        {
            Round();

            for (size_t i = 0; i < _sides.size(); ++i)
            {
                counts[i] = GetUnitCount(i);
            }
            auto it = std::find(counts.begin(), counts.end(), 0);
            if (it != counts.end())
            {
                return 1 - (it - counts.begin());
            }
            if (counts == prev_counts)
            {
                return -1;
            }
            counts.swap(prev_counts);
        }
    }

    int GetUnitCount(size_t side) const
    {
        auto &groups = _sides[side];
        return std::accumulate(groups.begin(), groups.end(), 0,
                               [](int a, const auto &g) {
                                    return a + g.count;
                               });
    }

private:
    struct _Group
    {
        size_t id;
        int count;
        int hp;
        int damage;
        std::string attack_type;
        int initiative;
        std::unordered_set<std::string> immunity, weakness;

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

suite s = [] {
    "2018-24"_test = [] {
        static constexpr char const *const TEST = R"(Immune System:
17 units each with 5390 hit points (weak to radiation, bludgeoning) with an attack that does 4507 fire damage at initiative 2
989 units each with 1274 hit points (immune to fire; weak to bludgeoning, slashing) with an attack that does 25 slashing damage at initiative 3

Infection:
801 units each with 4706 hit points (weak to radiation) with an attack that does 116 bludgeoning damage at initiative 1
4485 units each with 2961 hit points (immune to radiation; weak to fire, cold) with an attack that does 12 slashing damage at initiative 4
)";

        {
            Fight f(std::istringstream{TEST});
            f.Round();
            expect(1_u == f.RunUntilVictory());
            expect(0_i == f.GetUnitCount(0));
            expect(5216_i == f.GetUnitCount(1));
        }

        {
            Fight f(std::ifstream{INPUT});
            auto win = f.RunUntilVictory();
            expect(0_i == f.GetUnitCount(1 - win));
            Printer::Print(__FILE__, "1", f.GetUnitCount(win));
        }

        {
            Fight f(std::istringstream{TEST});
            f.Boost(0, 1570);
            expect(0_u == f.RunUntilVictory());
            expect(0_i == f.GetUnitCount(1));
            expect(51_i == f.GetUnitCount(0));
        }

        {
            const Fight original(std::ifstream{INPUT});

            auto calc = [&](int val) {
                Fight f(original);
                f.Boost(0, val);
                if (0 == f.RunUntilVictory())
                {
                    Printer::Print(__FILE__, "2", f.GetUnitCount(0));
                    return true;
                }
                return false;
            };

            for (int i = 1;; ++i)
            {
                if (calc(i))
                {
                    break;
                }
            }
        }
    };
};

} //namespace;