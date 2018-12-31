#include <doctest/doctest.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <iostream>

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

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
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
        REQUIRE(72 == c.Eval("d"));
        REQUIRE(507 == c.Eval("e"));
        REQUIRE(492 == c.Eval("f"));
        REQUIRE(114 == c.Eval("g"));
        REQUIRE(65412 == c.Eval("h"));
        REQUIRE(65079 == c.Eval("i"));
        REQUIRE(123 == c.Eval("x"));
        REQUIRE(456 == c.Eval("y"));
    }

    SUBCASE("task") {
        Calc c(std::ifstream{INPUT});
        auto val = c.Eval("a");
        MESSAGE(val);
        c.Reset();
        MESSAGE(c.Eval("a"));
    }
}
