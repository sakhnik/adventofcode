#include "../test.hpp"
#include <sstream>
#include <fstream>
#include <functional>

namespace {

void Anyone(std::string &accum, const std::string &value)
{
    std::string combined;
    std::set_union(accum.begin(), accum.end(),
                   value.begin(), value.end(),
                   std::back_insert_iterator(combined));
    swap(combined, accum);
}

void Everyone(std::string &accum, const std::string &value)
{
    auto it = std::set_intersection(accum.begin(), accum.end(),
                                    value.begin(), value.end(),
                                    accum.begin());
    accum.erase(it, accum.end());
}

int Count(std::istream &&is, std::function<void(std::string &, const std::string &)> policy)
{
    int count{};

    bool new_group{true};
    std::string answers;
    std::string line;
    while (getline(is, line))
    {
        if (line.empty())
        {
            count += answers.size();
            answers.clear();
            new_group = true;
            continue;
        }

        std::sort(line.begin(), line.end());
        if (new_group)
        {
            swap(answers, line);
            new_group = false;
            continue;
        }

        policy(answers, line);
    }
    count += answers.size();

    return count;
}

using namespace boost::ut;

suite s = [] {
    "06"_test = [] {
        const char *const TEST = R"(abc

a
b
c

ab
ac

a
a
a
a

b)";
        //expect(11_i == Count(std::istringstream{TEST}, Anyone));
        expect(6_i == Count(std::istringstream{TEST}, Everyone));

        Printer::Print(__FILE__, "1", Count(std::ifstream{INPUT}, Anyone));
        Printer::Print(__FILE__, "2", Count(std::ifstream{INPUT}, Everyone));
    };
};

} //namespace;
