#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>
#include <array>
#include "../test.hpp"

namespace {

using namespace boost::ut;

int Solve(std::istream &&is, std::initializer_list<int> init)
{
	std::array<int, 4> regs;
	std::copy(begin(init), end(init), begin(regs));

	struct Op
	{
		enum {
			CPY = 0,
			INC,
			DEC,
			JNZ,
			TGL
		} code;
		bool a_is_reg;
		int a;
		bool b_is_reg;
		int b;
	};

	std::vector<Op> program;

	std::string line;
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
		else
		{
			expect(false >> fatal) << line;
		}
	}

	for (ssize_t pc{0}; pc >= 0 && pc < (ssize_t) program.size(); ++pc)
	{
		Op op = program[pc];
		switch (op.code)
		{
		case Op::CPY:
			if (op.b_is_reg)
				regs[op.b] = op.a_is_reg ? regs[op.a] : op.a;
			break;
		case Op::INC:
			assert(op.a_is_reg);
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
					}
				}
			}
			break;
		}
	}

	return regs[0];
}

suite s = [] {
	"23"_test = [] {
		const char *const test1 =
			"cpy 41 a\n"
			"inc a\n"
			"inc a\n"
			"dec a\n"
			"jnz a 2\n"
			"dec a\n";
		expect(42_i == Solve(std::istringstream{test1}, {0, 0, 0, 0}));

		const char *const test2 =
			"cpy 2 a\n"
			"tgl a\n"
			"tgl a\n"
			"tgl a\n"
			"cpy 1 a\n"
			"dec a\n"
			"dec a\n";
		expect(3_i == Solve(std::istringstream{test2}, {0, 0, 0, 0}));

		if (Printer::EnableAll())
		{
			Printer::Print(__FILE__, "1", Solve(std::ifstream{INPUT}, {7, 0, 0, 0}));
			Printer::Print(__FILE__, "2", Solve(std::ifstream{INPUT}, {12, 0, 0, 0}));
		}
	};
};

} //namespace;
