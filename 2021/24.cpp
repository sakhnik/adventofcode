#include "../test.hpp"

namespace {

class Alu
{
public:
    static int Calc(const char *input)
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

        int x{}, y{}, z{}, w{};

        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 10;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 13;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 13;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 10;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 13;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 3;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -11;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 1;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 11;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 9;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -4;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 3;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 12;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 5;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 12;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 1;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 15;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 0;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -2;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 13;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -5;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 7;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -11;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 15;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -13;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 12;
        y *= x;
        z += y;
        w = *input++ - '0';
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -10;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 8;
        y *= x;
        z += y;

        return z;
    }

};


using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-24"_test = [] {

        expect(-159735204_i == Alu::Calc("00000000000000"));
        expect(-159735203_i == Alu::Calc("00000000000001"));
        expect(-159735202_i == Alu::Calc("00000000000002"));
        expect(161537073_i == Alu::Calc("11111111111111"));
        expect(482809480_i == Alu::Calc("22222222222222"));
        expect(804081887_i == Alu::Calc("33333333333333"));
        expect(1125354294_i == Alu::Calc("44444444444444"));
        expect(1446626701_i == Alu::Calc("55555555555555"));
        expect(1767899108_i == Alu::Calc("66666666666666"));
        expect(2089171515_i == Alu::Calc("77777777777777"));
        expect(-1884523248_i == Alu::Calc("88888888888888"));
        expect(-1563250844_i == Alu::Calc("99999999999996"));
        expect(-1563250843_i == Alu::Calc("99999999999997"));
        expect(-1563250842_i == Alu::Calc("99999999999998"));
        expect(-1563250841_i == Alu::Calc("99999999999999"));
        //Printer::Print(__FILE__, "1", Amphipods::Arrange(task));
        //Printer::Print(__FILE__, "2", Amphipods::Arrange(task2));
    };
};

} //namespace;
