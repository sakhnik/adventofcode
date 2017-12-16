#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <cassert>
#include <fstream>

std::string Dance(unsigned count, std::istream &is)
{
	std::string row;
	for (unsigned i = 0; i < count; ++i)
		row.push_back('a' + i);

	while (is)
	{
		char cmd{0};
		is >> cmd;
		switch (cmd)
		{
		case 's':
			{
				unsigned i{0};
				is >> i;
				row = row.substr(row.size() - i) + row.substr(0, row.size() - i);
				assert(row.size() == count);
			}
			break;
		case 'x':
			{
				unsigned i{0}, j{0};
				is >> i >> cmd >> j;
				std::swap(row[i], row[j]);
			}
			break;
		case 'p':
			{
				char a{0}, b{0};
				is >> a >> cmd >> b;
				auto i = row.find(a);
				auto j = row.find(b);
				std::swap(row[i], row[j]);
			}
			break;
		default:
			assert(!"Not implemented");
		}

		is >> cmd;  // comma
	}

	return row;
}

std::string Dance(unsigned count, std::istream &&is)
{
	return Dance(count, is);
}

TEST_CASE("main")
{
	REQUIRE(Dance(5, std::istringstream("s1,x3/4,pe/b")) == "baedc");

	std::ifstream ifs(INPUT);
	std::cout << Dance(16, ifs) << std::endl;
}
