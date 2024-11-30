#include "../test.hpp"
#include <unordered_set>
#include <fstream>
#include <cassert>

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

class Digit
{
public:
    using PatternsT = std::vector<std::string>;

    Digit()
    {
        _PrecalcCodes();
    }

    int SumNumbers(std::istream &&is)
    {
        int res{};

        std::string line;
        while (getline(is, line))
            res += FindMapping(line);

        return res;
    }

    int FindMapping(const std::string &line)
    {
        PatternsT patterns;
        int res{};

        std::istringstream iss{line};
        std::string word;
        bool bar{false};
        std::string mapping;
        while (iss >> word)
        {
            if (!bar)
            {
                if (word == "|")
                {
                    mapping = DeduceMappping(patterns);
                    bar = true;
                    continue;
                }
                patterns.push_back(word);
                continue;
            }
            res = res * 10 + CalcDigit(word, mapping);
        }

        return res;
    }

    std::string DeduceMappping(PatternsT &patterns)
    {
        std::sort(patterns.begin(), patterns.end(),
                [](const auto &a, const auto &b) { return a.size() < b.size(); });
        std::string mapping{"abcdefg"};
        do
        {
            auto code = Encode(mapping, patterns);
            if (_CODES.contains(code))
            {
                return mapping;
            }
        }
        while (std::next_permutation(mapping.begin(), mapping.end()));
        throw std::runtime_error("Mapping not found");
    }

    static int CalcDigit(const std::string &code, const std::string &mapping)
    {
        int num{};
        for (char c : code)
            num |= 1 << (mapping[c - 'a'] - 'a');
        auto it = std::find(DISP.begin(), DISP.end(), num);
        assert(it != DISP.end());
        return it - DISP.begin();
    }

    static std::string Encode(const std::string &mapping, const PatternsT &patterns)
    {
        std::string ret;
        for (const auto &p : patterns)
        {
            uint8_t digit{};
            for (auto c : p)
                digit |= 1 << (mapping[c - 'a'] - 'a');
            ret.push_back(digit);
        }
        return ret;
    }

    // 0:      1:      2:      3:      4:
    //  aaaa    ....    aaaa    aaaa    ....
    // b    c  .    c  .    c  .    c  b    c
    // b    c  .    c  .    c  .    c  b    c
    //  ....    ....    dddd    dddd    dddd
    // e    f  .    f  e    .  .    f  .    f
    // e    f  .    f  e    .  .    f  .    f
    //  gggg    ....    gggg    gggg    ....
    // 
    //   5:      6:      7:      8:      9:
    //  aaaa    aaaa    aaaa    aaaa    aaaa
    // b    .  b    .  .    c  b    c  b    c
    // b    .  b    .  .    c  b    c  b    c
    //  dddd    dddd    ....    dddd    dddd
    // .    f  e    f  .    f  e    f  .    f
    // .    f  e    f  .    f  e    f  .    f
    //  gggg    gggg    ....    gggg    gggg
    enum Segment
    {
        a = 1 << 0,
        b = 1 << 1,
        c = 1 << 2,
        d = 1 << 3,
        e = 1 << 4,
        f = 1 << 5,
        g = 1 << 6,
    };

    static constexpr std::array<int, 10> DISP = {
        a | b | c | e | f | g,      // 0
        c | f,                      // 1
        a | c | d | e | g,          // 2
        a | c | d | f | g,          // 3
        b | c | d | f,              // 4
        a | b | d | f | g,          // 5
        a | b | d | e | f | g,      // 6
        a | c | f,                  // 7
        a | b | c | d | e | f | g,  // 8
        a | b | c | d | f | g,      // 9
    };

    std::unordered_set<std::string> _CODES;

    void _PrecalcCodes()
    {
        PatternsT DIGITS = {
            "cf",       // 1
            "acf",      // 7
            "bcdf",     // 4
            "acdeg",    // 2
            "acdfg",    // 3
            "abdfg",    // 5
            "abcefg",   // 0
            "abdefg",   // 6
            "abcdfg",   // 9
            "abcdefg",  // 8
        };
        std::sort(DIGITS.begin() + 3, DIGITS.begin() + 6);
        std::sort(DIGITS.begin() + 6, DIGITS.begin() + 9);
        do
        {
            do
            {
                auto code = Encode("abcdefg", DIGITS);
                _CODES.insert(code);
            }
            while (std::next_permutation(DIGITS.begin() + 6, DIGITS.begin() + 9));
        }
        while (std::next_permutation(DIGITS.begin() + 3, DIGITS.begin() + 6));
    }
};

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
    "08"_test = [] {
        expect(26_u == CountUnique(std::istringstream{TEST_INPUT}));
        Printer::Print(__FILE__, "1", CountUnique(std::ifstream{INPUT}));

        using namespace std::string_literals;
        Digit digit;
        Digit::PatternsT patterns{"acedgfb", "cdfbe", "gcdfa", "fbcad", "dab", "cefabd", "cdfgeb", "eafb", "cagedb", "ab"};
        expect(eq(1, Digit::CalcDigit("ab", "cfgabde")));
        expect(eq("cfgabde"s, digit.DeduceMappping(patterns)));
        expect(5353_i == digit.FindMapping("acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab | cdfeb fcadb cdfeb cdbaf"));
        expect(61229_i == digit.SumNumbers(std::istringstream{TEST_INPUT}));
        Printer::Print(__FILE__, "2", digit.SumNumbers(std::ifstream{INPUT}));
    };
};

} //namespace;
