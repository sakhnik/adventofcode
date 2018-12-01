#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>
#include <array>
#include <functional>

using namespace std;

struct Op
{
	enum {
		CPY = 0,
		INC,
		DEC,
		JNZ,
		TGL,
		OUT,
	} code;
	bool a_is_reg;
	int a;
	bool b_is_reg;
	int b;
};

typedef vector<Op> ProgramT;

ProgramT ParseText(istream &is)
{
	ProgramT program;

	string line;
	while (getline(is, line))
	{
		char r1 = 0, r2 = 0;
		int val = 0, val2 = 0;
		if (2 == sscanf(line.c_str(), "cpy %d %c", &val, &r2))
			program.push_back({Op::CPY, false, val, true, r2 - 'a'});
		else if (2 == sscanf(line.c_str(), "cpy %c %c", &r1, &r2))
			program.push_back({Op::CPY, true, r1 - 'a', true, r2 - 'a'});
		else if (1 == sscanf(line.c_str(), "inc %c", &r1))
			program.push_back({Op::INC, true, r1 - 'a', false, 0});
		else if (1 == sscanf(line.c_str(), "dec %c", &r1))
			program.push_back({Op::DEC, true, r1 - 'a', false, 0});
		else if (2 == sscanf(line.c_str(), "jnz %d %d", &val2, &val))
			program.push_back({Op::JNZ, false, val2, false, val});
		else if (2 == sscanf(line.c_str(), "jnz %d %c", &val, &r1))
			program.push_back({Op::JNZ, false, val, true, r1 - 'a'});
		else if (2 == sscanf(line.c_str(), "jnz %c %d", &r1, &val))
			program.push_back({Op::JNZ, true, r1 - 'a', false, val});
		else if (1 == sscanf(line.c_str(), "tgl %c", &r1))
			program.push_back({Op::TGL, true, r1 - 'a', false, 0});
		else if (1 == sscanf(line.c_str(), "out %d", &val))
			program.push_back({Op::OUT, false, val, false, 0});
		else if (1 == sscanf(line.c_str(), "out %c", &r1))
			program.push_back({Op::OUT, true, r1 - 'a', false, 0});
		else
		{
			cout << line.c_str() << endl;
			assert(false);
		}
	}

	return program;
}

int Exec(ProgramT program, initializer_list<int> init, size_t iters, function<bool(const string &)> on_out)
{
	array<int, 4> regs;
	copy(begin(init), end(init), begin(regs));

	for (ssize_t pc{0}; pc >= 0 && pc < (ssize_t) program.size() && iters; ++pc, --iters)
	{
		Op op = program[pc];
		switch (op.code)
		{
		case Op::CPY:
			if (op.b_is_reg)
				regs[op.b] = op.a_is_reg ? regs[op.a] : op.a;
			break;
		case Op::INC:
			if (op.a_is_reg)
				++regs[op.a];
			break;
		case Op::DEC:
			assert(op.a_is_reg);
			--regs[op.a];
			break;
		case Op::JNZ:
			if (op.a_is_reg ? regs[op.a] : op.a)
				pc += (op.b_is_reg ? regs[op.b] : op.b) - 1;
			break;
		case Op::OUT:
			if (!on_out(std::to_string(op.a_is_reg ? regs[op.a] : op.a)))
				return -1;
			break;
		case Op::TGL:
			{
				assert(op.a_is_reg);
				auto x = (int)pc + regs[op.a];
				if (x > 0 && x < (int)program.size())
				{
					auto &instr = program[x];
					switch (instr.code)
					{
					case Op::INC: instr.code = Op::DEC; break;
					case Op::DEC: instr.code = Op::INC; break;
					case Op::JNZ: instr.code = Op::CPY; break;
					case Op::CPY: instr.code = Op::JNZ; break;
					case Op::TGL: instr.code = Op::INC; break;
					case Op::OUT: instr.code = Op::INC; break;
					}
				}
			}
			break;
		}
	}

	return regs[0];
}

int Solve(istream &&is)
{
	auto program = ParseText(is);

	try
	{
		for (int i = 0; i < 256; ++i)
		{
			int count = 12;
			bool exp_0 = true;

			auto on_out = [&](const auto &s)
			{
				for (auto ch : s)
				{
					if (exp_0)
					{
						if (ch != '0')
							return false;
					}
					else
					{
						if (ch != '1')
							return false;
					}

					exp_0 = !exp_0;
					if (!--count) throw i;
					return true;
				}

				return false;
			};
			Exec(program, {i,0,0,0}, 1000000000, on_out);
		}
	}
	catch (int i)
	{
		return i;
	}

	return -1;
}

TEST_CASE("main")
{
	cout << Solve(ifstream(INPUT)) << endl;
}
