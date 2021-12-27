#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

class Alu
{
public:
    static int64_t Calc(const char *input)
    {
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 1
        // add x 10
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 13
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 1
        // add x 13
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 10
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 1
        // add x 13
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 3
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 26
        // add x -11
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 1
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 1
        // add x 11
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 9
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 26
        // add x -4
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 3
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 1
        // add x 12
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 5
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 1
        // add x 12
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 1
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 1
        // add x 15
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 0
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 26
        // add x -2
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 13
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 26
        // add x -5
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 7
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 26
        // add x -11
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 15
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 26
        // add x -13
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 12
        // mul y x
        // add z y
        // inp w
        // mul x 0
        // add x z
        // mod x 26
        // div z 26
        // add x -10
        // eql x w
        // eql x 0
        // mul y 0
        // add y 25
        // mul y x
        // add y 1
        // mul z y
        // mul y 0
        // add y w
        // add y 8
        // mul y x
        // add z y

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

    static const std::function<int64_t(int64_t, int)> STAGES[14];

    static int64_t CalcChained(const char *input)
    {
        int64_t z{};
        for (auto f : STAGES)
            z = f(z, *input++ - '0');
        return z;
    }

    //static std::string Solve()
    //{
    //    using ZW = std::pair<int64_t, int>;
    //    using ValuesT = std::unordered_map<ZW, std::vector<int64_t>, boost::hash<ZW>>;
    //    ValuesT values;

    //    for (int64_t z = 0; z < 1000000; ++z)
    //        for (int w = 1; w <= 9; ++w)
    //        {
    //            int64_t zr = Stage14(z, w);
    //            values[{zr, w}].push_back(z);
    //        }
    //    return "";
    //}
};

const std::function<int64_t(int64_t, int)> Alu::STAGES[14] = {
    [](int64_t z, int w) {
        z = w + 13;
        return z;
    },
    [](int64_t z, int w) {
        z = z * 26 + w + 10;
        return z;
    },
    [](int64_t z, int w) {
        z = z * 26 + w + 3;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 - 11;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += x * (w + 1);
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 != w;
        z *= 25 * x + 1;
        z += (w + 9) * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 - 4;
        z /= 26;
        x = x != w;
        z *= 25 * x + 1;
        z += (w + 3) * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 + 12;
        x = x != w;

        z *= 25 * x + 1;
        z += (w + 5) * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 + 12;
        x = x != w;

        z *= 25 * x + 1;
        z += (w + 1) * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 + 15;
        x = x != w;

        z *= 25 * x + 1;
        z += w * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 - 2;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 13) * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 - 5;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 7) * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 - 11;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 15) * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = z % 26 - 13;
        x = x != w;

        z /= 26;
        z *= 25 * x + 1;
        z += (w + 12) * x;
        return z;
    },
    [](int64_t z, int w) {
        int64_t x = (z % 26 - 10) != w;
        z /= 26;
        z *= 25 * x + 1;
        z += (w + 8) * x;
        return z;
    },
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-24"_test = [] {
        expect(eq(4135231966ll, Alu::CalcChained("00000000000000")));
        expect(eq(4135231967ll, Alu::CalcChained("00000000000001")));
        expect(eq(159047383ll, Alu::CalcChained("00000000000002")));
        expect(eq(4456504373ll, Alu::CalcChained("11111111111111")));
        expect(eq(4777776780ll, Alu::CalcChained("22222222222222")));
        expect(eq(5099049187ll, Alu::CalcChained("33333333333333")));
        expect(eq(5420321594ll, Alu::CalcChained("44444444444444")));
        expect(eq(5741594001ll, Alu::CalcChained("55555555555555")));
        expect(eq(6062866408ll, Alu::CalcChained("66666666666666")));
        expect(eq(6384138815ll, Alu::CalcChained("77777777777777")));
        expect(eq(6705411222ll, Alu::CalcChained("88888888888888")));
        expect(eq(7026683626ll, Alu::CalcChained("99999999999996")));
        expect(eq(7026683627ll, Alu::CalcChained("99999999999997")));
        expect(eq(7026683628ll, Alu::CalcChained("99999999999998")));
        expect(eq(7026683629ll, Alu::CalcChained("99999999999999")));

 
        //Alu::Solve();
        //Printer::Print(__FILE__, "1", Amphipods::Arrange(task));
        //Printer::Print(__FILE__, "2", Amphipods::Arrange(task2));
    };
};

} //namespace;
