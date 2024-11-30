#include "../test.hpp"
#include <functional>
#include <fstream>

namespace {

class Computer
{
public:
    Computer(std::istream &is)
    {
        std::string line;
        while (getline(is, line))
        {
            _program.emplace_back(ParseLine(line));
        }
    }

    Computer(std::istream &&is)
        : Computer(is)
    {
    }

    void Run()
    {
        while (true)
        {
            if (_ip >= _program.size())
                break;
            const _OpT &op = _program[_ip];
            op();
        }
    }

    unsigned a = 0;
    unsigned b = 0;

private:
    size_t _ip = 0;

    using _OpT = std::function<void()>;
    std::vector<_OpT> _program;

    _OpT ParseLine(const std::string &line)
    {
        char c{};
        int off{};

        auto reg = [this](char c) -> unsigned& {
            return c == 'a' ? a : b;
        };

        if (1 == sscanf(line.c_str(), "hlf %c", &c))
        {
            return [this, &r = reg(c)]() {
                r /= 2;
                ++_ip;
            };
        }
        else if (1 == sscanf(line.c_str(), "tpl %c", &c))
        {
            return [this, &r = reg(c)]() {
                r *= 3;
                ++_ip;
            };
        }
        else if (1 == sscanf(line.c_str(), "inc %c", &c))
        {
            return [this, &r = reg(c)]() {
                ++r;
                ++_ip;
            };
        }
        else if (1 == sscanf(line.c_str(), "jmp %d", &off))
        {
            return [this, off]() {
                _ip += off;
            };
        }
        else if (2 == sscanf(line.c_str(), "jie %c, %d", &c, &off))
        {
            return [this, &r = reg(c), off]() {
                _ip += (r & 1) ? 1 : off;
            };
        }
        else if (2 == sscanf(line.c_str(), "jio %c, %d", &c, &off))
        {
            return [this, &r = reg(c), off]() {
                _ip += 1 == r ? off : 1;
            };
        }
        else
        {
            throw std::runtime_error("Not implemented");
        }
    }
};

using namespace boost::ut;

suite s = [] {
    "23"_test = [] {
        {
            Computer comp{std::istringstream{"inc a\njio a, +2\ntpl a\ninc a"}};
            comp.Run();
            expect(2_u == comp.a);
        }

        {
            Computer comp{std::ifstream{INPUT}};
            comp.Run();
            Printer::Print(__FILE__, "1", comp.b);
        }

        {
            Computer comp{std::ifstream{INPUT}};
            comp.a = 1;
            comp.Run();
            Printer::Print(__FILE__, "2", comp.b);
        }
    };
};

} //namespace;
