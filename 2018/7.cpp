#include <doctest/doctest.h>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

namespace {

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

struct Step
{
    char name;
    int deps_count = 0;
    std::vector<char> dependents;
};

using StepsMapT = std::unordered_map<char, Step>;

StepsMapT GetStepsMap(const DepsT &deps)
{
    StepsMapT steps_map;
    for (auto d : deps)
    {
        auto &s = steps_map[d.step];
        s.name = d.step;
        s.dependents.push_back(d.before);
        steps_map[d.before].name = d.before;
        ++steps_map[d.before].deps_count;
    }
    return steps_map;
}

class StepGenerator
{
public:
    StepGenerator(const DepsT &deps)
        : _steps_map{GetStepsMap(deps)}
    {
        for (const auto &kv : _steps_map)
        {
            _todo.push_back(kv.first);
        }
        _done = 0;
        _reported = 0;
        _Sort();
    }

    char GetNextStep()
    {
        if (_reported >= _todo.size())
        {
            return -1;
        }

        auto s = _todo[_reported];
        auto &step = _steps_map[s];
        if (!step.deps_count)
        {
            // The step can be executed now, because all its dependencies are now
            // satisfied.
            ++_reported;
            return s;
        }

        return -1;
    }

    void StepDone(char s)
    {
        auto it = std::find(begin(_todo) + _done, end(_todo), s);
        REQUIRE(it - begin(_todo) <= _reported);

        // The task is done, put it to the left.
        std::swap(_todo[_done], *it);
        ++_done;
        REQUIRE(_done <= _reported);

        // Update the dependent steps and resort.
        for (auto d : _steps_map[s].dependents)
        {
            --_steps_map[d].deps_count;
        }
        _Sort();
    }

    const std::string& GetSorted()
    {
        return _todo;
    }

private:
    StepsMapT _steps_map;  // Mutable information about the steps to do
    std::string _todo;     // Ordered steps
    size_t _done;          // The steps to the left are all done
    size_t _reported;      // The steps to the left are all reported

    void _Sort()
    {
        // Partition the steps not yet reported using their actual dependencies.
        auto it = std::partition(begin(_todo) + _reported, end(_todo),
                                 [&](char a) { return _steps_map[a].deps_count == 0; });
        // Sort then the ready steps by their name.
        std::sort(begin(_todo) + _reported, it);
    }
};

std::string SortSteps(const DepsT &deps)
{
    StepGenerator gen(deps);
    while (true)
    {
        auto s = gen.GetNextStep();
        if (s == -1)
        {
            break;
        }
        gen.StepDone(s);
    }
    return gen.GetSorted();
}

int Simulate(int concurrency, int time_add, const DepsT &deps)
{
    StepGenerator gen(deps);
    struct Worker
    {
        char step;
        int time_left = 0;
    };
    std::vector<Worker> workers;

    int time{0};
    while (true)
    {
        auto it = std::remove_if(begin(workers), end(workers),
                                 [&](auto &w) {
                                    if (--w.time_left > 0)
                                        return false;
                                    gen.StepDone(w.step);
                                    return true;
                                 });
        workers.erase(it, end(workers));

        for (int i = size(workers); i < concurrency; ++i)
        {
            char s = gen.GetNextStep();
            if (-1 == s)
            {
                break;
            }
            workers.push_back({s, time_add + 1 + s - 'A'});
        }

        if (workers.empty())
            break;

        ++time;
    }
    return time;
}

} //namespace;

TEST_CASE(TEST_NAME)
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

    REQUIRE(15 == Simulate(2, 0, test));
    std::cout << Simulate(5, 60, input) << std::endl;
}
