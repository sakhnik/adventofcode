#include "../test.hpp"
#include <fstream>

namespace {

static constexpr const char *const MINERALS[] = {"ore", "clay", "obsidian", "geode"};
static constexpr int COUNT = std::size(MINERALS);

struct Cost
    : std::array<int, COUNT>
{
    Cost()
    {
        fill(0);
    }

    Cost(std::initializer_list<int> vals)
    {
        fill(0);
        std::copy(vals.begin(), vals.end(), begin());
    }

    bool CanBuild(const Cost &price) const
    {
        for (int i = 0; i < size(); ++i)
        {
            if (price[i] && price[i] > (*this)[i])
                return false;
        }
        return true;
    }

    int NextBuild(const Cost &price, const Cost &robots) const
    {
        int next{};
        for (int i = 0; i < size(); ++i)
        {
            if (!price[i])
                continue;
            if (!robots[i])
                return -1;
            next = std::max(next, (price[i] - (*this)[i] + robots[i] - 1) / robots[i]);
        }
        return next;
    }

    Cost operator-(const Cost &o) const
    {
        Cost res;
        for (int i = 0; i < size(); ++i)
            res[i] = (*this)[i] - o[i];
        return res;
    }

    Cost operator+(const Cost &o) const
    {
        Cost res;
        for (int i = 0; i < size(); ++i)
            res[i] = (*this)[i] + o[i];
        return res;
    }

    Cost operator*(int factor) const
    {
        Cost res{*this};
        for (int i = 0; i < size(); ++i)
            res[i] *= factor;
        return res;
    }

    Cost Add(int i) const
    {
        Cost res{*this};
        ++res[i];
        return res;
    }
};

//inline std::ostream& operator<<(std::ostream &os, const Cost &cost)
//{
//    os << "(";
//    const char *space = "";
//    for (auto c : cost)
//    {
//        os << space << c;
//        space = " ";
//    }
//    return os << ")";
//}

struct Minerals
{
    struct Blueprint
    {
        std::vector<Cost> costs;

        Blueprint()
        {
            costs.resize(std::size(MINERALS));
        }
    };

    std::vector<Blueprint> blueprints;

    static int GetIdx(std::string_view mineral)
    {
        return std::find(std::begin(MINERALS), std::end(MINERALS), mineral) - std::begin(MINERALS);
    }

    Minerals(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            Blueprint blueprint;

            int idx{};
            char *sentence{};
            char *s = line.data();
            while ((sentence = strtok_r(s, ":.", &s)))
            {
                if (++idx < 2)
                    continue;

                int name_idx{};
                int amount{};
                int i{0};
                char *word{};
                char *s2 = sentence;
                while ((word = strtok_r(s2, " ", &s2)))
                {
                    switch (i++)
                    {
                    case 0:  // Each
                        continue;
                    case 1:
                        name_idx = GetIdx(word);
                        break;
                    case 2:  // robot
                    case 3:  // costs
                        continue;
                    case 4:
                        sscanf(word, "%d", &amount);
                        continue;
                    case 5:
                        {
                            int idx = GetIdx(word);
                            blueprint.costs[name_idx][idx] = amount;
                        }
                        continue;
                    case 6:  // and
                        i = 4;
                        break;
                    }
                }
            }
            blueprints.push_back(std::move(blueprint));
        }
    }

    int FindGeodes(const Blueprint &bp, int target_time)
    {
        // No need to build too many robots
        Cost max_robots{0};
        for (int i = 0; i < COUNT; ++i)
            for (int j = 0; j < COUNT; ++j)
                max_robots[j] = std::max(max_robots[j], bp.costs[i][j]);
        max_robots.back() = std::numeric_limits<int>::max();

        std::unordered_map<size_t, int> maximums;  // robots -> max geode

        auto search = [&](int time, const Cost &robots, const Cost &minerals, auto &&search) {

            if (time > target_time)
                return;

            auto it = maximums.find(time);
            if (it != maximums.end() && minerals.back() < it->second)
                return;
            maximums[time] = minerals.back();

            //std::cout << "t=" << time << ": robots=" << robots << " minerals=" << minerals << std::endl;
            if (time == target_time)
                return;

            int next_event{100};
            for (int i = bp.costs.size() - 1; i >= 0; --i)
            {
                if (robots[i] >= max_robots[i])
                    continue;
                const auto &cost = bp.costs[i];
                int next = minerals.NextBuild(cost, robots);
                if (next < 0)
                    continue;
                next_event = std::min(next_event, next);
                auto next_robots = robots.Add(i);
                auto next_minerals = minerals - cost + robots * (next + 1);
                search(time + next + 1, next_robots, next_minerals, search);
            }

            int eta = target_time - time;
            next_event = std::min(eta, next_event);
            if (next_event)
                search(time + next_event, robots, minerals + robots * next_event, search);
        };
        search(0, Cost{1}, Cost{0}, search);

        return maximums[target_time];
    }

    int Task1()
    {
        int bp_quality{};
        for (int i = 0; i < blueprints.size(); ++i)
            bp_quality += FindGeodes(blueprints[i], 24) * (i + 1);
        return bp_quality;
    }

    int Task2()
    {
        size_t res{1};
        for (int i = 0; i < 3; ++i)
            res *= FindGeodes(blueprints[i], 32);
        return res;
    }
};

const char *const TEST = R"(Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian.
Blueprint 2: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 8 clay. Each geode robot costs 3 ore and 12 obsidian.)";

using namespace boost::ut;

suite s = [] {
    "2022-19"_test = [] {
        Minerals test{std::istringstream{TEST}};
        expect(33_i == test.Task1());

        Minerals minerals{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", minerals.Task1());
        Printer::Print(__FILE__, "2", minerals.Task2());
    };
};

} //namespace;
