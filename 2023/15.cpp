#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Hash
{
    int task1{};

    Hash(std::istream &&is)
    {
        std::string step;
        while (getline(is, step, ','))
        {
            if (step.ends_with("\n"))
                step.pop_back();
            task1 += Calc(step);
        }
    }

    int Task1() const { return task1; }

    static int Calc(std::string_view s, int val = 0)
    {
        for (int ch : s)
            val = (val + ch) * 17;
        return val & 0xff;
    }
};

suite s = [] {
    "2023-15"_test = [] {
        expect(52_i == Hash::Calc("HASH"));
        const char *const TEST1 = R"(rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7)";
        Hash test1{std::istringstream{TEST1}};
        expect(1320_i == test1.Task1());
//        expect(64_i == test1.Task2());

        Hash hash{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", hash.Task1());
//        Printer::Print(__FILE__, "2", stones.Task2());
    };
};

} //namespace;
