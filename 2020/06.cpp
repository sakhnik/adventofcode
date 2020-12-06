#include "../test.hpp"
#include <sstream>
#include <fstream>

namespace {

int Count(std::istream &&is)
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

        std::string combined;
        std::set_union(answers.begin(), answers.end(),
                       line.begin(), line.end(),
                       std::back_insert_iterator(combined));
        swap(combined, answers);
    }
    count += answers.size();

    return count;
}

using namespace boost::ut;

suite s = [] {
    "2020-06"_test = [] {
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
        expect(11_i == Count(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", Count(std::ifstream{INPUT}));
    };
};

} //namespace;
