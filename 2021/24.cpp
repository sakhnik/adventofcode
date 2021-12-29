#include "../test.hpp"
#include <boost/range/irange.hpp>

namespace {

class Alu
{
public:
    static int64_t Calc(const char *input)
    {
        // Translate into C++:
        // '<,'>s/inp \(.\)/\1 = *input++ - '0';/g
        // '<,'>s/mul \([^ ]\+\) \([^ ]\+\)/\1 *= \2;/g
        // '<,'>s/add \([^ ]\+\) \([^ ]\+\)/\1 += \2;/g
        // '<,'>s/div \([^ ]\+\) \([^ ]\+\)/\1 \/= \2;/g
        // '<,'>s/mod \([^ ]\+\) \([^ ]\+\)/\1 %= \2;/g
        // '<,'>s/eql \([^ ]\+\) \([^ ]\+\)/\1 = \1 == \2;/g

        int64_t x{}, /*y{},*/ z{}, w{};

        w = *input++ - '0';
        z = w + 13;
        w = *input++ - '0';
        z = z * 26 + w + 10;
        w = *input++ - '0';
        z = z * 26 + w + 3;
        w = *input++ - '0';
        x = z % 26 - 11;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += x * (w + 1);
        w = *input++ - '0';
        x = z % 26 != w;
        z *= 25 * x + 1;
        z += (w + 9) * x;

        w = *input++ - '0';
        x = z % 26 - 4;
        z /= 26;
        x = x != w;
        z *= 25 * x + 1;
        z += (w + 3) * x;

        w = *input++ - '0';
        x = z % 26 + 12;
        x = x != w;

        z *= 25 * x + 1;
        z += (w + 5) * x;

        w = *input++ - '0';
        x = z % 26 + 12;
        x = x != w;

        z *= 25 * x + 1;
        z += (w + 1) * x;

        w = *input++ - '0';
        x = z % 26 + 15;
        x = x != w;

        z *= 25 * x + 1;
        z += w * x;

        w = *input++ - '0';
        x = z % 26 - 2;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 13) * x;

        w = *input++ - '0';
        x = z % 26 - 5;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 7) * x;

        w = *input++ - '0';
        x = z % 26 - 11;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 15) * x;

        w = *input++ - '0';
        x = z % 26 - 13;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 12) * x;

        w = *input++ - '0';
        x = (z % 26 - 10) != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 8) * x;

        return z;
    }

    std::vector<std::string> lines;

    Alu(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
            lines.push_back(line);
    }

    int GetArg(size_t i) const
    {
        const auto &line = lines[i];
        auto last = line.find_last_of(" ");
        return std::stoi(line.substr(last + 1));
    }

    std::string Find(bool biggest = true) const
    {
        const int COUNT = 14;
        const auto N = lines.size() / COUNT;
        std::string digits(COUNT, '0');
        std::vector<std::pair<int, int>> st;

        for (int i = 0; i < COUNT; i++)
        {
            auto off = i * N;
            if (GetArg(off + 4) == 1)
            {
                auto add2 = GetArg(off + 15);
                st.emplace_back(i, add2);
                continue;
            }

            auto add1 = GetArg(off + 5);
            auto [num, add2] = st.back();
            st.pop_back();
            auto delta = add1 + add2;
            for (char digit : biggest ? boost::irange<char>('9', '1' - 1, -1) : boost::irange<char>('1', '9' + 1, 1))
            {
                auto digit2 = digit + delta;
                if (digit2 >= '1' && digit2 <= '9')
                {
                    digits[num] = digit;
                    digits[i] = digit2;
                    break;
                }
            }
        }
        return digits;
    }
};


using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-24"_test = [] {
        expect(eq(4135231966ll, Alu::Calc("00000000000000")));
        expect(eq(4135231967ll, Alu::Calc("00000000000001")));
        expect(eq(159047383ll, Alu::Calc("00000000000002")));
        expect(eq(4456504373ll, Alu::Calc("11111111111111")));
        expect(eq(4777776780ll, Alu::Calc("22222222222222")));
        expect(eq(5099049187ll, Alu::Calc("33333333333333")));
        expect(eq(5420321594ll, Alu::Calc("44444444444444")));
        expect(eq(5741594001ll, Alu::Calc("55555555555555")));
        expect(eq(6062866408ll, Alu::Calc("66666666666666")));
        expect(eq(6384138815ll, Alu::Calc("77777777777777")));
        expect(eq(6705411222ll, Alu::Calc("88888888888888")));
        expect(eq(7026683626ll, Alu::Calc("99999999999996")));
        expect(eq(7026683627ll, Alu::Calc("99999999999997")));
        expect(eq(7026683628ll, Alu::Calc("99999999999998")));
        expect(eq(7026683629ll, Alu::Calc("99999999999999")));

        Alu alu{std::ifstream{INPUT}};
        expect(1 == alu.GetArg(4));
        expect(10 == alu.GetArg(5));
        expect(13 == alu.GetArg(15));
        Printer::Print(__FILE__, "1", alu.Find(true));
        Printer::Print(__FILE__, "2", alu.Find(false));
    };
};

} //namespace;
