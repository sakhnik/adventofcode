#include <doctest/doctest.h>
#include <fstream>
#include <unordered_map>
#include <regex>

namespace {

class Ticket
{
public:
    using TicketT = std::unordered_map<std::string, std::function<bool(int)>>;
    const TicketT& GetTicket0() const { return _ticket0; }
    const TicketT& GetTicket1() const { return _ticket1; }

    static int Match(const TicketT &ticket, std::istream &&is)
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
                if (!ticket.find(m[1])->second(std::stoi(m[2])))
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
    TicketT _ticket0 = {
        {"children", [](int v) { return v == 3; }},
        {"cats", [](int v) { return v == 7; }},
        {"samoyeds", [](int v) { return v == 2; }},
        {"pomeranians", [](int v) { return v == 3; }},
        {"akitas", [](int v) { return v == 0; }},
        {"vizslas", [](int v) { return v == 0; }},
        {"goldfish", [](int v) { return v == 5; }},
        {"trees", [](int v) { return v == 3; }},
        {"cars", [](int v) { return v == 2; }},
        {"perfumes", [](int v) { return v == 1; }},
    };
    TicketT _ticket1 = {
        {"children", [](int v) { return v == 3; }},
        {"cats", [](int v) { return v > 7; }},
        {"samoyeds", [](int v) { return v == 2; }},
        {"pomeranians", [](int v) { return v < 3; }},
        {"akitas", [](int v) { return v == 0; }},
        {"vizslas", [](int v) { return v == 0; }},
        {"goldfish", [](int v) { return v < 5; }},
        {"trees", [](int v) { return v > 3; }},
        {"cars", [](int v) { return v == 2; }},
        {"perfumes", [](int v) { return v == 1; }},
    };
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("task") {
        Ticket t;
        MESSAGE(t.Match(t.GetTicket0(), std::ifstream{INPUT}));
        MESSAGE(t.Match(t.GetTicket1(), std::ifstream{INPUT}));
    }
}
