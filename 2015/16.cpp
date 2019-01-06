#include <doctest/doctest.h>
#include <fstream>
#include <unordered_map>
#include <regex>

namespace {

class Ticket
{
public:
    int Match(std::istream &&is) const
    {
        std::string line;
        while (is && getline(is, line))
        {
            int num{};
            sscanf(line.c_str(), "Sue %d", &num);

            bool matched{true};

            static const std::regex r(R"((\w+): (\d+))");
            using IterT = std::regex_iterator<std::string::const_iterator>;
            for (IterT it(line.begin(), line.end(), r), itend; it != itend; ++it)
            {
                const auto &m = *it;
                if (_ticket.find(m[1])->second != std::stoi(m[2]))
                {
                    matched = false;
                    break;
                }
            }

            if (matched)
            {
                return num;
            }
        }

        return -1;
    }

private:
    std::unordered_map<std::string, int> _ticket = {
        {"children", 3},
        {"cats", 7},
        {"samoyeds", 2},
        {"pomeranians", 3},
        {"akitas", 0},
        {"vizslas", 0},
        {"goldfish", 5},
        {"trees", 3},
        {"cars", 2},
        {"perfumes", 1},
    };
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("task") {
        Ticket t;
        MESSAGE(t.Match(std::ifstream{INPUT}));
    }
}
