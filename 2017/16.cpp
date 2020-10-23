#include <cassert>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>
#include "../test.hpp"

namespace {

struct Instr
{
    enum class Op
    {
        S, X, P
    };

    Op op;
    union
    {
        char c;
        unsigned u;
    } a1;
    union
    {
        char c;
        unsigned u;
    } a2;
};

typedef std::vector<Instr> ProgramT;

ProgramT Parse(std::istream &&is)
{
    ProgramT program;
    while (is)
    {
        char cmd{0};
        is >> cmd;
        Instr instr;
        switch (cmd)
        {
        case 's':
            instr.op = Instr::Op::S;
            is >> instr.a1.u;
            break;
        case 'x':
            instr.op = Instr::Op::X;
            is >> instr.a1.u >> cmd >> instr.a2.u;
            break;
        case 'p':
            instr.op = Instr::Op::P;
            is >> instr.a1.c >> cmd >> instr.a2.c;
            break;
        default:
            assert(!"Not implemented");
        }

        is >> cmd;  // comma
        program.push_back(instr);
    }
    return program;
}

std::string& Dance(std::string &row, const ProgramT &program)
{
    for (auto instr : program)
    {
        switch (instr.op)
        {
        case Instr::Op::S:
            {
                unsigned pivot = row.size() - instr.a1.u;
                std::rotate(row.begin(), row.begin() + pivot, row.end());
                break;
            }
        case Instr::Op::X:
            {
                std::swap(row[instr.a1.u], row[instr.a2.u]);
                break;
            }
        case Instr::Op::P:
            {
                auto i = row.find(instr.a1.c);
                auto j = row.find(instr.a2.c);
                std::swap(row[i], row[j]);
                break;
            }
        }
    }

    return row;
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2017-16"_test = [] {
        auto test_program = Parse(std::istringstream("s1,x3/4,pe/b"));
        std::string test_row = "abcde";
        expect(eq(Dance(test_row, test_program), "baedc"s));

        std::string row;
        for (int i = 0; i < 16; ++i)
            row.push_back('a' + i);

        // Apparently, the transformations are cyclic.
        // So let's detect the start and period of the loop.
        std::vector<std::string> variations;
        variations.push_back(row);

        size_t loop{0};
        auto program = Parse(std::ifstream(INPUT));
        while (true)
        {
            Dance(row, program);
            auto it = std::find(variations.begin(), variations.end(), row);
            if (it != variations.end())
            {
                loop = it - variations.begin();
                break;
            }
            variations.push_back(row);
        }

        Printer::Print(__FILE__, "1", variations[1]);
        auto period = variations.size() - loop;
        Printer::Print(__FILE__, "2", variations[loop + ((1000000000 - loop) % period)]);
    };
};

} //namespace;