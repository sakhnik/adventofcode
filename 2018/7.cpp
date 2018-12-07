#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <sstream>
#include <unordered_map>
#include <fstream>

struct Dep
{
    char step;
    char before;
};

using DepsT = std::vector<Dep>;

DepsT GetInput(std::istream &&is)
{
    DepsT deps;

    std::string line;
    while (is && getline(is, line))
    {
        char s = line[5];
        char before = line[36];
        deps.push_back({s, before});
    }

    return deps;
}

std::string SortSteps(DepsT &deps)
{
    struct Step
    {
        char name;
        int deps = 0;
        std::vector<char> notify;
    };

    std::unordered_map<char, Step> steps_map;
    for (auto d : deps)
    {
        auto &s = steps_map[d.step];
        s.name = d.step;
        s.notify.push_back(d.before);
        steps_map[d.before].name = d.before;
        ++steps_map[d.before].deps;
    }

    // Copy the data into a vector for sorting.
    std::vector<Step*> steps;
    for (auto &s : steps_map)
    {
        steps.push_back(&s.second);
    }

    auto order = [](const Step *a, const Step *b) {
        // First come the steps with fewer dependent.
        // NOTE the reversed order, because the heap is max-heap.
        if (a->deps > b->deps)
            return true;
        if (a->deps < b->deps)
            return false;
        // Otherwise order them by name.
        return a->name > b->name;
    };

    std::make_heap(begin(steps), end(steps), order);
    std::string ret;

    for (auto it = end(steps); it != begin(steps); )
    {
        std::pop_heap(begin(steps), it, order);
        --it;
        auto &step = *it;
        ret.push_back(step->name);
        for (char b : step->notify)
        {
            --steps_map[b].deps;
        }
        std::make_heap(begin(steps), it, order);
    }

    return ret;
}

TEST_CASE("main")
{
    const auto TEST = R"(Step C must be finished before step A can begin.
Step C must be finished before step F can begin.
Step A must be finished before step B can begin.
Step A must be finished before step D can begin.
Step B must be finished before step E can begin.
Step D must be finished before step E can begin.
Step F must be finished before step E can begin.)";
    auto test = GetInput(std::istringstream(TEST));
    REQUIRE("CABDFE" == SortSteps(test));

    auto input = GetInput(std::ifstream(INPUT));
    std::cout << SortSteps(input) << std::endl;
}
