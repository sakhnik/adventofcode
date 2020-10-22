#include <fstream>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "../test.hpp"
namespace {

class Calc
{
public:
    Calc(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            char a[64], b[64], c[64];
            if (2 == sscanf(line.c_str(), "%s -> %s", a, b))
            {
                _wires[b] = [this, a]() {
                    return _Eval(a);
                };
            }
            else if (2 == sscanf(line.c_str(), "NOT %s -> %s", a, b))
            {
                _wires[b] = [this, a]() {
                    return ~_Eval(a);
                };
            }
            else if (3 == sscanf(line.c_str(), "%s AND %s -> %s", a, b, c))
            {
                _wires[c] = [this, a, b]() {
                    return _Eval(a) & _Eval(b);
                };
            }
            else if (3 == sscanf(line.c_str(), "%s OR %s -> %s", a, b, c))
            {
                _wires[c] = [this, a, b]() {
                    return _Eval(a) | _Eval(b);
                };
            }
            else if (3 == sscanf(line.c_str(), "%s LSHIFT %s -> %s", a, b, c))
            {
                _wires[c] = [this, a, b]() {
                    return _Eval(a) << _Eval(b);
                };
            }
            else if (3 == sscanf(line.c_str(), "%s RSHIFT %s -> %s", a, b, c))
            {
                _wires[c] = [this, a, b]() {
                    return _Eval(a) >> _Eval(b);
                };
            }
        }
    }

    uint16_t Eval(const std::string &a)
    {
        return _wires[a].Eval();
    }

    void Reset()
    {
        uint16_t v = Eval("a");
        for (auto &w : _wires)
        {
            w.second.Reset();
        }
        _wires["b"].Set(v);
    }

private:
    using _EvalT = std::function<uint16_t()>;
    struct _Node
    {
        _Node() = default;

        template <typename E>
        _Node(E e) : eval(e) { }

        _EvalT eval;
        uint16_t val{};
        bool processed = false;

        uint16_t Eval()
        {
            if (!processed)
            {
                val = eval();
                processed = true;
            }
            return val;
        }

        void Reset()
        {
            processed = false;
        }

        void Set(uint16_t v)
        {
            processed = true;
            val = v;
        }
    };

    std::unordered_map<std::string, _Node> _wires;

    uint16_t _Eval(const std::string &e)
    {
        auto it = _wires.find(e);
        if (it != _wires.end())
            return it->second.Eval();
        return std::stod(e);
    }
};

using namespace boost::ut;

suite s = [] {
    "2015-07.test"_test = [] {
        const char *const TEST =
            "123 -> x\n"
            "456 -> y\n"
            "x AND y -> d\n"
            "x OR y -> e\n"
            "x LSHIFT 2 -> f\n"
            "y RSHIFT 2 -> g\n"
            "NOT x -> h\n"
            "NOT y -> i\n";
        Calc c(std::istringstream{TEST});
        expect(72_i == c.Eval("d"));
        expect(507_i == c.Eval("e"));
        expect(492_i == c.Eval("f"));
        expect(114_i == c.Eval("g"));
        expect(65412_i == c.Eval("h"));
        expect(65079_i == c.Eval("i"));
        expect(123_i == c.Eval("x"));
        expect(456_i == c.Eval("y"));
    };

    "2015-07.task"_test = [] {
        Calc c(std::ifstream{INPUT});
        auto val = c.Eval("a");
        Printer::Print(__FILE__, "1", val);
        c.Reset();
        Printer::Print(__FILE__, "2", c.Eval("a"));
    };
};

} //namespace;