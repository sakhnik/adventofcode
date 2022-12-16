#include "../test.hpp"
#include <fstream>

namespace {

struct Pipes
{
    struct Valve
    {
        int index{};
        int flow = 0;
        std::vector<std::string> exits;
    };
    std::unordered_map<std::string, Valve> valves;
    std::map<int, std::string> good_valves;
    std::vector<int> dist;
    static constexpr int MAX_DIST = std::numeric_limits<int>::max() / 3;

    int DistIdx(int from, int to) const
    {
        return from * valves.size() + to;
    }

    Pipes(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            char name[64];
            Valve valve;
            valve.index = valves.size();
            sscanf(line.c_str(), "Valve %s has flow rate=%d;", name, &valve.flow);
            const std::string EXITS = "tunnels lead to valves ";
            auto st = line.find(EXITS);
            if (line.npos != st)
            {
                std::istringstream iss(line.c_str() + st + EXITS.size());
                while (iss >> line)
                {
                    if (line.back() == ',')
                        line.erase(line.size() - 1);
                    valve.exits.push_back(line);
                }
            }
            else
            {
                const std::string EXIT = "tunnel leads to valve ";
                auto st = line.find(EXIT);
                line = line.c_str() + st + EXIT.size();
                valve.exits.push_back(line);
            }

            valves[name] = std::move(valve);
        }

        dist.resize(valves.size() * valves.size(), MAX_DIST);
        for (int i = 0; i < valves.size(); ++i)
            dist[DistIdx(i, i)] = 0;

        std::vector<std::string> good_valves;
        for (auto &[name, valve] : valves)
        {
            if (valve.flow)
                good_valves.push_back(name);
            for (const auto &ex : valve.exits)
                dist[DistIdx(valve.index, valves.at(ex).index)] = 1;
        }

        // Floyd-Warshall
        for (int k = 0; k < valves.size(); ++k)
        {
            for (int i = 0; i < valves.size(); ++i)
            {
                for (int j = 0; j < valves.size(); ++j)
                {
                    auto sum = dist[DistIdx(i, k)] + dist[DistIdx(k, j)];
                    auto &d = dist[DistIdx(i, j)];
                    if (d > sum)
                        d = sum;
                }
            }
        }

        for (int i = 0; i < good_valves.size(); ++i)
            this->good_valves[i] = good_valves[i];
    }

    using OpenValvesT = std::bitset<16>;
    using MaxPressureT = std::unordered_map<OpenValvesT, int>;

    void Walk(const std::string &v, int time, OpenValvesT open_valves, int pressure, MaxPressureT &max_pressure)
    {
        auto it = max_pressure.find(open_valves);
        max_pressure[open_valves] = std::max(pressure, it != max_pressure.end() ? it->second : 0);

        const auto &valve = valves.at(v);
        for (const auto &[i, next] : good_valves)
        {
            const auto &next_valve = valves.at(next);
            int d = dist[DistIdx(valve.index, next_valve.index)];
            int new_time = time - d - 1;
            if (open_valves.test(i) || new_time < 0)
                continue;
            Walk(next, new_time, open_valves.to_ulong() | (1 << i), pressure + next_valve.flow * new_time, max_pressure);
        }
    }

    int Task1()
    {
        MaxPressureT max_pressure;
        Walk("AA", 30, 0, 0, max_pressure);
        return std::max_element(max_pressure.begin(), max_pressure.end(),
                [](const auto &a, const auto &b) { return a.second < b.second; })->second;
    }

    int Task2()
    {
        MaxPressureT max_pressure;
        Walk("AA", 26, 0, 0, max_pressure);

        int res{};
        for (const auto &[path1, pressure1] : max_pressure)
        {
            for (const auto &[path2, pressure2] : max_pressure)
            {
                if ((path1.to_ulong() & path2.to_ulong()))
                    continue;
                res = std::max(res, pressure1 + pressure2);
            }
        }
        return res;
    }
};

const char *const TEST = R"(Valve AA has flow rate=0; tunnels lead to valves DD, II, BB
Valve BB has flow rate=13; tunnels lead to valves CC, AA
Valve CC has flow rate=2; tunnels lead to valves DD, BB
Valve DD has flow rate=20; tunnels lead to valves CC, AA, EE
Valve EE has flow rate=3; tunnels lead to valves FF, DD
Valve FF has flow rate=0; tunnels lead to valves EE, GG
Valve GG has flow rate=0; tunnels lead to valves FF, HH
Valve HH has flow rate=22; tunnel leads to valve GG
Valve II has flow rate=0; tunnels lead to valves AA, JJ
Valve JJ has flow rate=21; tunnel leads to valve II
)";

using namespace boost::ut;

suite s = [] {
    "2022-16"_test = [] {
        Pipes test{std::istringstream{TEST}};
        expect(1651_i == test.Task1());
        expect(1707_i == test.Task2());

        Pipes pipes{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", pipes.Task1());
        Printer::Print(__FILE__, "2", pipes.Task2());
    };
};

} //namespace;
