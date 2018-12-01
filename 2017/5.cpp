#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>

template <typename ModifierT>
unsigned CountJumps(std::vector<int> offsets, ModifierT modifier)
{
	unsigned count{0};
	unsigned ip{0};

	try
	{
		while (true)
		{
			auto &offset = offsets.at(ip);
			ip += modifier(offset);
			++count;
		}
	}
	catch (const std::out_of_range &)
	{
		return count;
	}
}

int IncreaseModifier(int &offset)
{
	return offset++;
}

int WeirdModifier(int &offset)
{
	return offset >= 3 ? offset-- : offset++;
}

TEST_CASE("main")
{
	REQUIRE(CountJumps({0, 3, 0, 1, -3}, IncreaseModifier) == 5);

	std::ifstream ifs(INPUT);
	std::vector<int> offsets;
	int o{0};
	while (ifs >> o)
		offsets.emplace_back(o);

	std::cout << CountJumps(offsets, IncreaseModifier) << std::endl;
	std::cout << CountJumps(offsets, WeirdModifier) << std::endl;
}
