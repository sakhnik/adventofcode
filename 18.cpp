#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <unordered_map>
#include <fstream>

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
		Mul,
		Mod,
		Rcv,
		Jgz
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

		program.push_back(instr);
	}

	return program;
}

ProgramT Parse(std::istream &&is)
{
	return Parse(is);
}

int64_t Execute(const ProgramT &program, bool debug = false)
{
	std::unordered_map<char, int64_t> regs;
	int64_t last{0};

	auto get_value = [&](Arg a) {
		if (a.is_reg)
			return regs[a.a.reg];
		return a.a.num;
	};

	auto print_arg = [&](std::ostream &os, Arg a) -> std::ostream& {
		os << ' ';
		if (a.is_reg)
			return os << a.a.reg << "(" << regs[a.a.reg] << ")";
		return os << a.a.num;
	};

	for (size_t i = 0; i < program.size(); ++i)
	{
		using Op = Instr::Op;
		auto instr = program[i];
		auto X = instr.X;
		auto Y = instr.Y;

		switch (instr.op)
		{
		case Op::Snd:
			//snd X plays a sound with a frequency equal to the value of X.
			last = get_value(X);
			if (debug)
			{
				std::cout << i << ": snd";
				print_arg(std::cout, X) << std::endl;
			}
			break;
		case Op::Set:
			//set X Y sets register X to the value of Y.
			if (debug)
			{
				std::cout << i << ": set " << X.a.reg;
				print_arg(std::cout, Y) << std::endl;
			}
			regs[X.a.reg] = get_value(Y);
			break;
		case Op::Add:
			//add X Y increases register X by the value of Y.
			if (debug)
			{
				std::cout << i << ": add " << X.a.reg;
				print_arg(std::cout, Y) << std::endl;
			}
			regs[X.a.reg] += get_value(Y);
			break;
		case Op::Mul:
			//mul X Y sets register X to the result of multiplying the value contained in register X by the value of Y.
			if (debug)
			{
				std::cout << i << ": mul " << X.a.reg;
				print_arg(std::cout, Y) << std::endl;
			}
			regs[X.a.reg] *= get_value(Y);
			break;
		case Op::Mod:
			//mod X Y sets register X to the remainder of dividing the value contained in register X by the value of Y (that is, it sets X to the result of X modulo Y).
			if (debug)
			{
				std::cout << i << ": mod " << X.a.reg;
				print_arg(std::cout, Y) << std::endl;
			}
			regs[X.a.reg] %= get_value(Y);
			break;
		case Op::Rcv:
			//rcv X recovers the frequency of the last sound played, but only when the value of X is not zero. (If it is zero, the command does nothing.)
			if (debug)
			{
				std::cout << i << ": rcv";
				print_arg(std::cout, X) << std::endl;
			}
			if (get_value(X) != 0)
			{
				return last;
			}

			break;
		case Op::Jgz:
			//jgz X Y jumps with an offset of the value of Y, but only if the value of X is greater than zero. (An offset of 2 skips the next instruction, an offset of -1 jumps to the previous instruction, and so on.)
			if (debug)
			{
				std::cout << i << ": jgz";
				print_arg(std::cout, X);
				print_arg(std::cout, Y) << std::endl;
			}
			if (get_value(X) > 0)
				i += get_value(Y) - 1;
			break;
		}
	}

	return last;
}

TEST_CASE("main")
{
	const char *const test_program = R"(set a 1
add a 2
mul a a
mod a 5
snd a
set a 0
rcv a
jgz a -1
set a 1
jgz a -2)";
	auto test = Parse(std::istringstream(test_program));
	REQUIRE(Execute(test) == 4);

	auto program = Parse(std::ifstream(INPUT));
	std::cout << Execute(program) << std::endl;
}
