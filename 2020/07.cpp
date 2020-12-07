#include "../test.hpp"
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <queue>

namespace {

using namespace boost::ut;

class Bags
{
public:
    Bags(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            const std::string CONTAIN = " bags contain ";
            auto cont = line.find(CONTAIN);
            expect(cont != line.npos);
            std::string outer = line.substr(0, cont);
            std::string_view rest{line.data() + cont + CONTAIN.size()};
            if (rest.starts_with("no other bags"))
            {
                _children[outer];
                continue;
            }

            for (int i = 0; ; ++i)
            {
                auto j = rest.find_first_of(",.");
                if (j == rest.npos)
                    break;
                //3 faded blue bags
                auto bag = rest.substr(0, j);
                rest = rest.substr(j + 1 + (rest[j] == ','));

                auto first_space = bag.find(' ');
                expect(first_space != bag.npos);
                //auto num = bag.substr(0, first_space);
                auto last_space = bag.rfind(' ');
                expect(last_space != bag.npos && last_space != first_space);
                std::string desc{bag.substr(first_space + 1, last_space - first_space - 1)};
                _children[outer].insert(std::string{desc});
                _parents[desc].insert(std::string{outer});
            }
        }
    }

    size_t CountOuter(const std::string &color) const
    {
        BagsT bags;
        std::queue<std::string> q;
        q.push(color);
        while (!q.empty())
        {
            std::string c = q.front();
            q.pop();
            auto it = _parents.find(c);
            if (it == _parents.end())
                continue;

            for (const auto &can_contain : it->second)
            {
                if (!bags.contains(can_contain))
                {
                    bags.insert(can_contain);
                    q.push(can_contain);
                }
            }
        }
        return bags.size();
    }

private:
    using BagsT = std::unordered_set<std::string>;
    std::unordered_map<std::string, BagsT> _children;
    std::unordered_map<std::string, BagsT> _parents;
};

suite s = [] {
    "2020-07"_test = [] {
        {
            const char *const TEST = ""
                R"(light red bags contain 1 bright white bag, 2 muted yellow bags.
dark orange bags contain 3 bright white bags, 4 muted yellow bags.
bright white bags contain 1 shiny gold bag.
muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
dark olive bags contain 3 faded blue bags, 4 dotted black bags.
vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
faded blue bags contain no other bags.
dotted black bags contain no other bags.)";
            Bags bags{std::istringstream{TEST}};
            expect(4_u == bags.CountOuter("shiny gold"));
        }

        Bags bags{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", bags.CountOuter("shiny gold"));
    };
};

} //namespace;
