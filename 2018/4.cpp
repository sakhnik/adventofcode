#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <regex>
#include <unordered_map>
#include <fstream>

struct Guard
{
    int id;
    int hours_asleep = 0;
    std::array<uint8_t, 60> activity;
};
using StatsT = std::vector<Guard>;

void Print(const StatsT &stats)
{
    for (const auto &s : stats)
    {
        printf("%4d  %2d  ", s.id, s.hours_asleep);
        for (auto c : s.activity)
        {
            printf("%c", c ? '#' : '.');
        }
        printf("\n");
    }
}

StatsT GetInput(std::istream &&is)
{
    StatsT stats;

    static const std::regex start(R"(\[....-..-.. ..:..\] Guard #(\d+) .*)");
    static const std::regex falls(R"(\[....-..-.. ..:(..)\] falls asleep)");
    static const std::regex wakes(R"(\[....-..-.. ..:(..)\] wakes up)");

    Guard cur_guard;
    bool started{false};
    int sleep_start{};
    std::string line;

    auto pushGuard = [&] {
        if (started)
        {
            stats.push_back(std::move(cur_guard));
        }
        cur_guard = Guard{};
        started = true;
    };

    while (is && getline(is, line))
    {
        std::smatch m;
        if (std::regex_match(line, m, start))
        {
            pushGuard();
            cur_guard.id = std::stoi(m[1]);
        }
        else if (std::regex_match(line, m, falls))
        {
            sleep_start = std::stoi(m[1]);
        }
        else if (std::regex_match(line, m, wakes))
        {
            int sleep_end = std::stoi(m[1]);
            cur_guard.hours_asleep += sleep_end - sleep_start;
            for (int i = sleep_start; i < sleep_end; ++i)
            {
                cur_guard.activity[i] = 1;
            }
        }
        else
        {
            REQUIRE(false);
        }
    }

    pushGuard();

    return stats;
}

int FindTheChampion(const StatsT &stats)
{
    std::unordered_map<int, int> guards;
    for (const auto &s : stats)
    {
        guards[s.id] += s.hours_asleep;
    }
    auto champ = std::max_element(begin(guards), end(guards), [](auto a, auto b) { return a.second < b.second; });
    return champ->first;
}

int GuessTheHour(const StatsT &stats, int id)
{
    std::array<int, 60> mins{};
    for (const auto &s : stats)
    {
        if (id != s.id)
        {
            continue;
        }

        for (size_t i = 0; i < size(mins); ++i)
        {
            mins[i] += s.activity[i];
        }
    }

    auto it = std::max_element(begin(mins), end(mins));
    return it - mins.begin();
}

TEST_CASE("main")
{
    const auto TEST_INPUT = R"([1518-11-01 00:00] Guard #10 begins shift
[1518-11-01 00:05] falls asleep
[1518-11-01 00:25] wakes up
[1518-11-01 00:30] falls asleep
[1518-11-01 00:55] wakes up
[1518-11-01 23:58] Guard #99 begins shift
[1518-11-02 00:40] falls asleep
[1518-11-02 00:50] wakes up
[1518-11-03 00:05] Guard #10 begins shift
[1518-11-03 00:24] falls asleep
[1518-11-03 00:29] wakes up
[1518-11-04 00:02] Guard #99 begins shift
[1518-11-04 00:36] falls asleep
[1518-11-04 00:46] wakes up
[1518-11-05 00:03] Guard #99 begins shift
[1518-11-05 00:45] falls asleep
[1518-11-05 00:55] wakes up)";
    auto test_stats = GetInput(std::istringstream(TEST_INPUT));
    //Print(test_stats);

    REQUIRE(10 == FindTheChampion(test_stats));
    REQUIRE(24 == GuessTheHour(test_stats, 10));

    auto stats = GetInput(std::ifstream(INPUT));
    auto champ = FindTheChampion(stats);
    std::cout << champ * GuessTheHour(stats, champ) << std::endl;
}
