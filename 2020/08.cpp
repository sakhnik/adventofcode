#include "../test.hpp"
#include <functional>
#include <unordered_set>
#include <fstream>

namespace {

class Comp
{
public:
    Comp(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            int arg{};
            if (1 == sscanf(line.c_str(), "acc %d", &arg))
                _program.emplace_back([this, arg] { _a += arg; ++_ip; });
            else if (1 == sscanf(line.c_str(), "jmp %d", &arg))
                _program.emplace_back([this, arg] { _ip += arg; });
            else // nop
                _program.emplace_back([this] { ++_ip; });
        }
    }

    int RunUntilLoop()
    {
        std::unordered_set<size_t> history;
        while (!history.contains(_ip))
        {
            history.insert(_ip);
            _program.at(_ip)();
        }
        return _a;
    }

private:
    using CmdT = std::function<void(void)>;
    std::vector<CmdT> _program;
    size_t _ip = 0;
    int _a = 0;
};

using namespace boost::ut;

suite s = [] {
    "2020-08"_test = [] {
        {
            const char *const TEST = ""
                R"(nop +0
acc +1
jmp +4
acc +3
jmp -3
acc -99
acc +1
jmp -4
acc +6)";
            expect(5_i == Comp{std::istringstream{TEST}}.RunUntilLoop());
        }

        Comp comp{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", comp.RunUntilLoop());
    };
};

} //namespace;
