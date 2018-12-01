#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>

unsigned Sum(std::string_view s, unsigned dist)
{
	unsigned sum{0};

	for (size_t i = 0, n = s.size(); i != n; ++i)
	{
		if (s[i] == s[(i+dist) % s.size()])
			sum += s[i] - '0';
	}

	return sum;
}

TEST_CASE("main")
{
	REQUIRE(Sum("1122", 1) == 3);
	REQUIRE(Sum("1111", 1) == 4);
	REQUIRE(Sum("1234", 1) == 0);
	REQUIRE(Sum("91212129", 1) == 9);
	REQUIRE(Sum("1212", 2) == 6);
	REQUIRE(Sum("1221", 2) == 0);
	REQUIRE(Sum("123425", 3) == 4);
	REQUIRE(Sum("123123", 3) == 12);
	REQUIRE(Sum("12131415", 4) == 4);

	std::string input;
	getline(std::ifstream(INPUT), input);
	std::cout << Sum(input, 1) << std::endl;
	std::cout << Sum(input, input.size() / 2) << std::endl;
}
