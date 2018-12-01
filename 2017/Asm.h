#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

struct Arg
{
    bool is_reg = false;
    union
    {
        int64_t num;
        char reg;
    } a;
};

struct Instr
{
    enum class Op
    {
        Snd,
        Set,
        Add,
        Sub,
        Mul,
        Mod,
        Rcv,
        Jgz,
        Jnz,
    } op;
    Arg X, Y;
};

typedef std::vector<Instr> ProgramT;

Arg ParseArg(std::istream &is)
{
    Arg arg;

    std::string s;
    is >> s;
    arg.is_reg = isalpha(s[0]);
    if (arg.is_reg)
    {
        arg.a.reg = s[0];
    }
    else
    {
        arg.a.num = std::stoi(s);
    }

    return arg;
}

ProgramT Parse(std::istream &is)
{
    ProgramT program;

    std::string line;
    while (getline(is, line))
    {
        Instr instr;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "snd")
        {
            instr.op = Instr::Op::Snd;
            instr.X = ParseArg(iss);
        }
        else if (cmd == "set")
        {
            instr.op = Instr::Op::Set;
            instr.X = ParseArg(iss);
            instr.Y = ParseArg(iss);
        }
        else if (cmd == "add")
        {
            instr.op = Instr::Op::Add;
            instr.X = ParseArg(iss);
            instr.Y = ParseArg(iss);
        }
        else if (cmd == "sub")
        {
            instr.op = Instr::Op::Sub;
            instr.X = ParseArg(iss);
            instr.Y = ParseArg(iss);
        }
        else if (cmd == "mul")
        {
            instr.op = Instr::Op::Mul;
            instr.X = ParseArg(iss);
            instr.Y = ParseArg(iss);
        }
        else if (cmd == "mod")
        {
            instr.op = Instr::Op::Mod;
            instr.X = ParseArg(iss);
            instr.Y = ParseArg(iss);
        }
        else if (cmd == "rcv")
        {
            instr.op = Instr::Op::Rcv;
            instr.X = ParseArg(iss);
        }
        else if (cmd == "jgz")
        {
            instr.op = Instr::Op::Jgz;
            instr.X = ParseArg(iss);
            instr.Y = ParseArg(iss);
        }
        else if (cmd == "jnz")
        {
            instr.op = Instr::Op::Jnz;
            instr.X = ParseArg(iss);
            instr.Y = ParseArg(iss);
        }

        program.push_back(instr);
    }

    return program;
}

ProgramT Parse(std::istream &&is)
{
    return Parse(is);
}
