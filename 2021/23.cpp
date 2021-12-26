#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

class Amphipods
{
public:

    struct Config
    {
        std::string hallway = "...........";
        std::array<std::string, 4> rooms = {"AA", "BB", "CC", "DD"};
        int capacity = 2;

        Config(int capacity = 2)
            : capacity{capacity}
        {
            for (int i = 0; i < rooms.size(); ++i)
                rooms[i].resize(capacity, 'A' + i);
        }

        bool operator==(const Config &o) const
        {
            return hallway == o.hallway && rooms == o.rooms;
        }

        void Print() const
        {
            std::cout << "--------------\n";
            std::cout << hallway << "\n";
            for (int r = 0; r < capacity; ++r)
            {
                std::cout << " ";
                for (int i = 0; i < rooms.size(); ++i)
                {
                    auto spaces = capacity - rooms[i].size();
                    std::cout << " " << (r >= spaces ? rooms[i][r - spaces] : '.');
                }
                std::cout << "\n";
            }
        }
    };

    struct ConfigHash
    {
        size_t operator()(const Config &c) const
        {
            size_t seed{};
            boost::hash_combine(seed, c.hallway);
            for (const auto &r : c.rooms)
                boost::hash_combine(seed, r);
            return seed;
        }
    };

    struct ConfigWeighted
        : Config
    {
        static constexpr std::array<size_t, 4> PRICE = {1, 10, 100, 1000};
        size_t cost = 0;

        std::vector<ConfigWeighted> Moves() const
        {
            std::vector<ConfigWeighted> moves;

            // Move to the room
            for (size_t i = 0; i < hallway.size(); ++i)
            {
                if (hallway[i] == '.')
                    continue;
                char kind = hallway[i];
                int idx = kind - 'A';
                int door = 2 + 2 * idx;
                if (rooms[idx].find_first_not_of(kind) != std::string::npos)
                    continue;

                int step = door > i ? 1 : -1;
                auto way_is_free = [&] {
                    for (int j = i + step; j != door; j += step)
                    {
                        if (hallway[j] != '.')
                            return false;
                    }
                    return true;
                };
                if (!way_is_free())
                    continue;

                ConfigWeighted new_config{*this};
                new_config.hallway[i] = '.';
                new_config.rooms[idx] = std::string{kind} + new_config.rooms[idx];
                new_config.cost = PRICE[idx] * (step * (door - i) + capacity - rooms[idx].size());
                moves.push_back(std::move(new_config));
            }

            // Move out of the room
            auto outOfRoom = [&](int i, std::initializer_list<std::initializer_list<int>> steps) {
                auto idx = rooms[i].find_first_not_of('A' + i);
                if (idx == std::string::npos)
                    return;

                ConfigWeighted new_config{*this};
                char kind = new_config.rooms[i].front();
                new_config.rooms[i].erase(0, 1);
                new_config.cost = (capacity - new_config.rooms[i].size()) * PRICE[kind - 'A'];

                int door = 2 + 2 * i;
                for (auto &s : steps)
                    for (auto j : s)
                    {
                        if (hallway[j] != '.')
                            break;
                        ConfigWeighted config{new_config};
                        config.hallway[j] = kind;
                        config.cost += PRICE[kind - 'A'] * std::abs(door - j);
                        moves.push_back(std::move(config));
                    }
            };

            outOfRoom(0, {{1, 0}, {3, 5, 7, 9, 10}});
            outOfRoom(1, {{3, 1, 0}, {5, 7, 9, 10}});
            outOfRoom(2, {{5, 3, 1, 0}, {7, 9, 10}});
            outOfRoom(3, {{7, 5, 3, 1, 0}, {9, 10}});

            return moves;
        }

        void Print() const
        {
            std::cout << "-----\n";
            Config::Print();
            std::cout << "cost = " << cost << std::endl;
        }
    };

    struct ConfigWeightedHash
    {
        size_t operator()(const ConfigWeighted &c) const
        {
            size_t seed{ConfigHash{}(c)};
            boost::hash_combine(seed, c.cost);
            return seed;
        }
    };

    static size_t Arrange(Config start)
    {
        struct Cost
        {
            size_t cost = std::numeric_limits<size_t>::max();
            Config from;
        };

        using ConfigsT = std::unordered_map<Config, Cost, ConfigHash>; 
        ConfigsT configs;
        configs[start].cost = 0;

        using ConfigsWeightedT = std::unordered_set<ConfigWeighted, ConfigWeightedHash>;
        ConfigsWeightedT visited;

        auto configOrder = [](const ConfigWeighted &a, const ConfigWeighted &b) {
            return a.cost > b.cost;
        };

        std::priority_queue<ConfigWeighted, std::vector<ConfigWeighted>, decltype(configOrder)> to_visit;
        ConfigWeighted start_weighted{start};
        start_weighted.cost = 0;
        to_visit.push(std::move(start_weighted));

        while (!to_visit.empty())
        {
            ConfigWeighted cfg = std::move(to_visit.top());
            to_visit.pop();
            visited.insert(cfg);
            auto cost = configs.at(cfg);

            for (auto &m : cfg.Moves())
            {
                if (!visited.contains(m))
                {
                    auto new_cost = cost.cost + m.cost;
                    auto it = configs.find(m);
                    if (it == configs.end() || it->second.cost > new_cost)
                    {
                        configs[m] = {new_cost, cfg};
                        to_visit.push(std::move(m));
                    }
                }
            }
        }

        //Config cfg{start.capacity};
        //int prev_cost = configs.at(cfg).cost;
        //while (true)
        //{
        //    cfg.Print();
        //    const auto &cost = configs.at(cfg);
        //    std::cout << (prev_cost - cost.cost) << " -> " << cost.cost << std::endl;
        //    if (!cost.cost)
        //        break;
        //    cfg = cost.from;
        //    prev_cost = cost.cost;
        //}

        return configs.at(Config{start.capacity}).cost;
    }
};


using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-23"_test = [] {

        // #############
        // #...........#
        // ###B#C#B#D###
        //   #A#D#C#A#
        //   #########
        Amphipods::Config test;
        test.rooms = {"BA", "CD", "BC", "DA"};
        expect(12521_u == Amphipods::Arrange(test));

        // #############
        // #...........#
        // ###D#A#D#C###
        //   #B#C#B#A#
        //   #########
        Amphipods::Config task;
        task.rooms = {"DB", "AC", "DB", "CA"};
        Printer::Print(__FILE__, "1", Amphipods::Arrange(task));

        Amphipods::Config test2(4);
        test2.rooms = {"BDDA", "CCBD", "BBAC", "DACA"};
        expect(44169_u == Amphipods::Arrange(test2));

        Amphipods::Config task2(4);
        task2.rooms = {"DDDB", "ACBC", "DBAB", "CACA"};
        Printer::Print(__FILE__, "2", Amphipods::Arrange(task2));
    };
};

} //namespace;
