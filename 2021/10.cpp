#include "../test.hpp"

namespace {

size_t CalcErrorScore(std::istream &&is)
{
    size_t score{};
    std::string line;
    while (is >> line)
    {
        std::stack<char> chunks;
        for (char ch : line)
        {
            auto checkClose = [&](char op, size_t sc) {
                if (chunks.top() != op)
                {
                    score += sc;
                    chunks.pop();
                    return true;
                }
                chunks.pop();
                return false;
            };

            if (ch == '{' || ch == '(' || ch == '[' || ch == '<')
                chunks.push(ch);
            else if (ch == ')')
            {
                if (checkClose('(', 3))
                    break;
            }
            else if (ch == ']')
            {
                if (checkClose('[', 57))
                    break;
            }
            else if (ch == '}')
            {
                if (checkClose('{', 1197))
                    break;
            }
            else if (ch == '>')
            {
                if (checkClose('<', 25137))
                    break;
            }
        }
    }
    return score;
}

const char *TEST_INPUT = R"(
[({(<(())[]>[[{[]{<()<>>
[(()[<>])]({[<{<<[]>>(
{([(<{}[<>[]}>{[]{[(<()>
(((({<>}<{<{<>}{[]{[]{}
[[<[([]))<([[{}[[()]]]
[{[{({}]{}}([{[{{{}}([]
{<[[]]>}<{[{[{[]{()[[[]
[<(<(<(<{}))><([]([]()
<{([([[(<>()){}]>(<<{{
<{([{{}}[<[[[<>{}]]]>[]]
)";

using namespace boost::ut;

suite s = [] {
    "2021-10"_test = [] {
        expect(26397_u == CalcErrorScore(std::istringstream{TEST_INPUT}));

        Printer::Print(__FILE__, "1", CalcErrorScore(std::ifstream{INPUT}));
    };
};

} //namespace;
