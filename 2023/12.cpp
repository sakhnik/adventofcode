#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Springs
{
    int task1{};

    Springs(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            auto sep = line.find(' ');
            auto pattern = line.substr(0, sep);
            std::istringstream iss{line.substr(sep + 1)};
            int run{};
            std::string runs;
            while (iss >> run)
            {
                runs.push_back(run);
                iss.get();
            }
            task1 += CountMatches(pattern, runs);
        }
    }

    int Task1() const
    {
        return task1;
    }

    static int CountMatches(std::string_view pattern, std::string_view runs)
    {
        while (pattern.front() == '.')
            pattern = pattern.substr(1);

        if (pattern.empty())
            return runs.empty();

        if (runs.empty())
            return pattern.find('#') == pattern.npos;

        int count{};
        if (pattern.front() == '?')
            count = CountMatches(pattern.substr(1), runs);

        int num = runs.front();
        if (pattern.size() < num)
            return 0;

        // Can we build a sequence long enough starting from the head of the pattern?
        auto idx = pattern.substr(0, num).find_first_not_of("#?");
        if (idx == pattern.npos) idx = num;
        if (idx >= num)
        {
            if (idx == pattern.size())
                count += CountMatches(pattern.substr(num), runs.substr(1));
            else if (pattern[idx] == '.' || pattern[idx] == '?')
                count += CountMatches(pattern.substr(num + 1), runs.substr(1));
        }

        return count;
    }
};

suite s = [] {
    "2023-12"_test = [] {
        const char *const TEST1 = R"(???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1
)";
        Springs test1{std::istringstream{TEST1}};
        expect(1_i == Springs::CountMatches("??", ""));
        expect(1_i == Springs::CountMatches("???.###", "\1\1\3"));
        expect(4_i == Springs::CountMatches(".??..??...?##.", "\1\1\3"));
        expect(1_i == Springs::CountMatches("?#?#?#?#?#?#?#?", "\1\3\1\6"));
        expect(1_i == Springs::CountMatches("????.#...#...", "\4\1\1"));
        expect(4_i == Springs::CountMatches("????.######..#####.", "\1\6\5"));
        expect(10_i == Springs::CountMatches("???????", "\2\1"));
        expect(10_i == Springs::CountMatches("?###????????", "\3\2\1"));
        expect(21_i == test1.Task1());
        //expect(1030_i == test1.Impl(10));
        //expect(8410_i == test1.Impl(100));

        Springs springs{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", springs.Task1());
        //Printer::Print(__FILE__, "2", cosmos.Impl(1000000));
    };
};

} //namespace;
