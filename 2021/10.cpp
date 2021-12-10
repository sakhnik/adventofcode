#include "../test.hpp"

namespace {

class Syntax
{
public:
    Syntax(std::istream &&is)
    {
        std::string line;
        while (is >> line)
        {
            std::stack<char> chunks;
            bool error = false;

            for (char ch : line)
            {
                auto checkClose = [&](char op, size_t score) {
                    if (chunks.top() != op)
                    {
                        _error_score += score;
                        chunks.pop();
                        error = true;
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

            if (error)
                continue;

            size_t score{};
            while (!chunks.empty())
            {
                score *= 5;
                switch (chunks.top())
                {
                case '(': score += 1; break;
                case '[': score += 2; break;
                case '{': score += 3; break;
                case '<': score += 4; break;
                }
                chunks.pop();
            }
            _completion.push_back(score);
        }
    }

    size_t GetErrorScore() const { return _error_score; }

    size_t GetCompletionScore()
    {
        auto mid = _completion.begin() + _completion.size() / 2;
        std::nth_element(_completion.begin(), mid, _completion.end());
        return *mid;
    }

private:
    size_t _error_score = 0;
    std::vector<size_t> _completion;
};

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
        Syntax test{std::istringstream{TEST_INPUT}};
        expect(26397_u == test.GetErrorScore());
        expect(288957_u == test.GetCompletionScore());

        Syntax input{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", input.GetErrorScore());
        Printer::Print(__FILE__, "2", input.GetCompletionScore());
    };
};

} //namespace;
