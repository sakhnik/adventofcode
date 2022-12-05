#include "../test.hpp"
#include <fstream>

namespace {

const char *const TEST =
"    [D]    \n"
"[N] [C]    \n"
"[Z] [M] [P]\n"
" 1   2   3 \n"
R"(
move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
)";

std::string Rearrange(std::istream &&is)
{
    std::vector<std::string> stacks;

    std::string line;
    while (std::getline(is, line))
    {
        if (line.empty())
            break;
        if (stacks.empty())
            stacks.resize((line.size() + 1) / 4);
        for (int i = 0; ; ++i)
        {
            int idx = 4 * i + 1;
            if (idx >= line.size())
                break;
            char kind = line[idx];
            if (kind >= 'A' && kind <= 'Z')
                stacks[i].push_back(kind);
        }
    }

    for (auto &s : stacks)
        std::reverse(s.begin(), s.end());

    while (std::getline(is, line))
    {
        int count{}, from{}, to{};
        sscanf(line.c_str(), "move %d from %d to %d", &count, &from, &to);
        while (count--)
        {
            stacks[to - 1].push_back(stacks[from - 1].back());
            stacks[from - 1].pop_back();
        }
    }

    std::string tops;
    for (auto &s : stacks)
    {
        if (!s.empty())
            tops.push_back(s.back());
    }
    return tops;
}

using namespace boost::ut;

suite s = [] {
    "2022-05"_test = [] {
        expect("CMZ" == Rearrange(std::istringstream{TEST}));

        Printer::Print(__FILE__, "1", Rearrange(std::ifstream{INPUT}));
    };
};

} //namespace;
