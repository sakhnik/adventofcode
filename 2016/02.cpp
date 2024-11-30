#include <sstream>
#include <fstream>
#include <string>
#include <cassert>
#include "../test.hpp"
namespace {

std::string Solve(std::istream &&is)
{
	std::string code;
	int cur{5};

	std::string line;
	while (getline(is, line))
	{
		for (char c : line)
		{
			switch (c)
			{
			case 'U':
				if (cur > 3) cur -= 3;
				break;
			case 'D':
				if (cur < 7) cur += 3;
				break;
			case 'L':
				if (cur % 3 != 1) --cur;
				break;
			case 'R':
				if (cur % 3) ++cur;
				break;
			}
			assert(cur >= 1 && cur <= 9);
		}

		code.push_back(cur + '0');
	}

	return code;
}

std::string Solve2(std::istream &&is)
{
	std::string code;

	char layout[5][5] = {
		{0,    0,  '1',  0,   0},
		{0,   '2', '3', '4',  0},
		{'5', '6', '7', '8', '9'},
		{0,   'A', 'B', 'C',  0},
		{0,    0,  'D',  0,   0}
	};

	int cur_x{0}, cur_y{2};

	std::string line;
	while (is >> line)
	{
		for (char ch : line)
		{
			switch (ch)
			{
			case 'U':
				if (!cur_y) continue;
				if (layout[cur_y - 1][cur_x])
					--cur_y;
				break;
			case 'D':
				if (cur_y == 4) continue;
				if (layout[cur_y + 1][cur_x])
					++cur_y;
				break;
			case 'L':
				if (!cur_x) continue;
				if (layout[cur_y][cur_x - 1])
					--cur_x;
				break;
			case 'R':
				if (cur_x == 4) continue;
				if (layout[cur_y][cur_x + 1])
					++cur_x;
				break;
			}
		}

		code.push_back(layout[cur_y][cur_x]);
	}

	return code;
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
	"02"_test = [] {
		expect(eq("1985"s, Solve(std::istringstream{"UUL\nRRDDD\nLURDL\nUUUUD"})));
		Printer::Print(__FILE__, "1", Solve(std::ifstream{INPUT}));

		expect(eq("5DB3"s, Solve2(std::istringstream{"UUL\nRRDDD\nLURDL\nUUUUD"})));
		Printer::Print(__FILE__, "2", Solve2(std::ifstream{INPUT}));
	};
};

} //namespace;
