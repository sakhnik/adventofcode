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

class StepGenerator
{
public:
    StepGenerator(const DepsT &deps)
        : _deps(deps)
    {
        for (auto d : deps)
        {
            _order[d.step];  // Make sure it's created
            ++_order[d.before];
        }
        for (const auto &kv : _order)
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
        if (!_order[s])
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
        for (auto d : _deps)
        {
            if (d.step == s)
            {
                --_order[d.before];
            }
        }
        _Sort();
    }

    const std::string& GetSorted()
    {
        return _todo;
    }

private:
    const DepsT &_deps;
    std::unordered_map<char, int> _order;  // Dependency count
    std::string _todo;     // Ordered steps
    size_t _done;          // The steps to the left are all done
    size_t _reported;      // The steps to the left are all reported

    void _Sort()
    {
        // Partition the steps not yet reported using their actual dependencies.
        auto it = std::partition(begin(_todo) + _reported, end(_todo),
                                 [&](char a) { return _order[a] == 0; });
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
    MESSAGE(SortSteps(input));

    REQUIRE(15 == Simulate(2, 0, test));
    MESSAGE(Simulate(5, 60, input));
}