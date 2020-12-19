#include "../test.hpp"
#include <charconv>
#include <sstream>

namespace {

size_t Count(std::istream &&is, bool subst = false)
{
    struct Rex
    {
        std::string rex;

        enum class Status
        {
            Unresolved = 0,
            Pending,
            Resolved
        } status = Status::Unresolved;
    };

    std::unordered_map<int, Rex> rules;

    std::string line;
    while (getline(is, line))
    {
        if (line.empty())
            break;
        char ch{};
        int num{};
        if (2 == sscanf(line.c_str(), "%d: \"%c\"", &num, &ch))
        {
            rules[num] = {{ch}, Rex::Status::Resolved};
        }
        else
        {
            std::string_view s{line};
            s = std::from_chars(s.begin(), s.end(), num).ptr + 1;

            std::string ss{s};
            if (subst)
            {
                // Part 2 adjustments
                if (num == 8)
                {
                    for (int i = 2; i < 10; ++i)
                    {
                        ss += " |";
                        for (int j = 0; j < i; ++j)
                            ss += " 42";
                    }
                }
                else if (num == 11)
                {
                    for (int i = 2; i < 5; ++i)
                    {
                        ss += " |";
                        for (int j = 0; j < i; ++j)
                            ss += " 42";
                        for (int j = 0; j < i; ++j)
                            ss += " 31";
                    }
                }
            }

            rules[num] = {ss, Rex::Status::Unresolved};
        }
    }

    std::function<bool(int)> resolve = [&](int num) -> bool {
        auto &rex = rules.find(num)->second;
        if (rex.status == Rex::Status::Resolved)
            return true;
        if (rex.status == Rex::Status::Pending)
            return false;

        rex.status = Rex::Status::Pending;

        std::string resolved;
        std::string_view s{rex.rex};

        std::string seq;

        auto combine = [&] {
            if (resolved.empty())
                resolved = '(' + seq;
            else
                resolved += '|' + seq;
            seq.clear();
        };

        while (!s.empty())
        {
            if (s[0] == ' ')
            {
                s = s.substr(1);
            }
            else if (s[0] == '|')
            {
                s = s.substr(1);
                combine();
            }
            else
            {
                int ref{};
                s = std::from_chars(s.begin(), s.end(), ref).ptr;
                if (!resolve(ref))
                {
                    rex.status = Rex::Status::Unresolved;
                    return false;
                }
                auto &ref_rex = rules.find(ref)->second;
                seq += ref_rex.rex;
            }
        }

        combine();
        if (resolved.front() == '(')
            resolved += ')';
        rex.rex = resolved;
        rex.status = Rex::Status::Resolved;
        return true;
    };

    // Didn't implement topological sort, so brute force.
    while (true)
    {
        bool finished{true};
        for (const auto &r : rules)
        {
            if (!resolve(r.first))
            {
                finished = false;
            }
        }
        if (finished)
            break;
    }

    //std::cout << rules[0].rex << std::endl;
    auto rex = std::regex(rules[0].rex);

    size_t count{};
    while (getline(is, line))
    {
        count += std::regex_match(line, rex);
    }
    return count;
}

using namespace boost::ut;

suite s = [] {
    "2020-19"_test = [] {
        {
            const char *const TEST = R"(0: 4 1 5
1: 2 3 | 3 2
2: 4 4 | 5 5
3: 4 5 | 5 4
4: "a"
5: "b"

ababbb
bababa
abbbab
aaabbb
aaaabbb)";
            expect(2_u == Count(std::istringstream{TEST}));
        }

        Printer::Print(__FILE__, "1", Count(std::ifstream{INPUT}));
        Printer::Print(__FILE__, "2", Count(std::ifstream{INPUT}, true));
    };
};

} //namespace;
