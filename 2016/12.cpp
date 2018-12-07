#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>
#include <array>

using namespace std;

int Solve(istream &&is, initializer_list<int> init)
{
	array<int, 4> regs;
	copy(begin(init), end(init), begin(regs));

	struct Op
	{
		enum {
			CPY_REG = 0,
			CPY_VAL,
			INC,
			DEC,
			JNZ_REG,
			JNZ_VAL
		} code;
		int a;
		int b;
	};

	vector<Op> program;

	string line;
	while (getline(is, line))
	{
		char r1 = 0, r2 = 0;
		int val = 0, val2 = 0;
		if (2 == sscanf(line.c_str(), "cpy %d %c", &val, &r2))
		{
			program.push_back({Op::CPY_VAL, val, r2 - 'a'});
			continue;
		}
		if (2 == sscanf(line.c_str(), "cpy %c %c", &r1, &r2))
		{
			program.push_back({Op::CPY_REG, r1 - 'a', r2 - 'a'});
			continue;
		}
		if (1 == sscanf(line.c_str(), "inc %c", &r1))
		{
			program.push_back({Op::INC, r1 - 'a', 0});
			continue;
		}
		if (1 == sscanf(line.c_str(), "dec %c", &r1))
		{
			program.push_back({Op::DEC, r1 - 'a', 0});
			continue;
		}
		if (2 == sscanf(line.c_str(), "jnz %d %d", &val2, &val))
		{
			program.push_back({Op::JNZ_VAL, val2, val});
			continue;
		}
		if (2 == sscanf(line.c_str(), "jnz %c %d", &r1, &val))
		{
			program.push_back({Op::JNZ_REG, r1 - 'a', val});
			continue;
		}
		assert(false);
	}

	for (size_t pc{0}; pc < program.size(); ++pc)
	{
		Op op = program[pc];
		switch (op.code)
		{
		case Op::CPY_REG:
			regs[op.b] = regs[op.a];
			break;
		case Op::CPY_VAL:
			regs[op.b] = op.a;
			break;
		case Op::INC:
			++regs[op.a];
			break;
		case Op::DEC:
			--regs[op.a];
			break;
		case Op::JNZ_REG:
			if (regs[op.a])
				pc += op.b - 1;
			break;
		case Op::JNZ_VAL:
			if (op.a)
				pc += op.b - 1;
			break;
		}
	}

	return regs[0];
}

TEST_CASE(TEST_NAME)
{
	const char *const test =
		"cpy 41 a\n"
		"inc a\n"
		"inc a\n"
		"dec a\n"
		"jnz a 2\n"
		"dec a\n";
	REQUIRE(42 == Solve(istringstream(test), {0,0,0,0}));

	cout << Solve(ifstream(INPUT), {0,0,0,0}) << endl;
	cout << Solve(ifstream(INPUT), {0,0,1,0}) << endl;
}
