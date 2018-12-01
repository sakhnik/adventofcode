#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <numeric>

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

TEST_CASE("main")
{
    auto test_program = Parse(std::istringstream("s1,x3/4,pe/b"));
    std::string test_row = "abcde";
    REQUIRE(Dance(test_row, test_program) == "baedc");

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

    std::cout << variations[1] << std::endl;
    auto period = variations.size() - loop;
    std::cout << variations[loop + ((1000000000 - loop) % period)] << std::endl;
}
