#include "../test.hpp"

namespace {

size_t CountUnique(std::istream &&is)
{
    size_t count{};

    std::string line;
    while (getline(is, line))
    {
        std::istringstream iss{line};
        std::string word;
        bool bar{false};
        while (iss >> word)
        {
            if (!bar)
            {
                if (word == "|")
                    bar = true;
                continue;
            }

            if (word.size() == 2 || word.size() == 3 || word.size() == 4 || word.size() == 7)
                ++count;
        }
    }

    return count;
}

const char *TEST_INPUT = R"(
be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe
edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec | fcgedb cgb dgebacf gc
fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef | cg cg fdcagb cbg
fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec cb
aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga | gecf egdcabf bgf bfgea
fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf | gebdcfa ecba ca fadegcb
dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | cefg dcbef fcge gbcadfe
bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd | ed bcgafe cdgba cbgef
egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | gbdfcae bgc cg cgb
gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc | fgae cfgab fg bagce
)";

using namespace boost::ut;

suite s = [] {
    "2021-08"_test = [] {
        expect(26_u == CountUnique(std::istringstream{TEST_INPUT}));

        Printer::Print(__FILE__, "1", CountUnique(std::ifstream{INPUT}));
    };
};

} //namespace;
