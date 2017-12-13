#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <fstream>

unsigned CalcSeverity(std::istream &is)
{
	std::vector<unsigned> depths;

	unsigned depth{0}, range{0};
	char skip;
	while (is >> depth >> skip >> range)
	{
		if (depths.size() < depth)
			depths.resize(depth);
		depths.push_back(range);
	}

	unsigned severity{0};

	for (size_t i = 0; i < depths.size(); ++i)
	{
		if (!depths[i])
			continue;
		unsigned period = 2 * (depths[i] - 1);
		if (i % period == 0)
		{
			severity += depths[i] * i;
		}
	}

	return severity;
}

unsigned CalcSeverity(std::istream &&is)
{
	return CalcSeverity(is);
}

TEST_CASE("main")
{
	REQUIRE(CalcSeverity(std::istringstream{"0: 3\n1: 2\n4: 4\n6: 4"}) == 24);

	std::ifstream ifs(INPUT);
	std::cout << CalcSeverity(ifs) << std::endl;
}
