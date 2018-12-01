#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>

std::pair<unsigned, unsigned> Count(const char *path)
{
	// A good guide to hexagonal grids is here:
	// https://www.redblobgames.com/grids/hexagons/
	// We'll use cube coordinates (a diagonal slice of cube is hex grid too)

	int x{0}, y{0}, z{0};
	auto calc_dist = [&] { return ((x < 0 ? -x : x) + (y < 0 ? -y : y) + (z < 0 ? -z : z)) / 2; };
	unsigned max_dist{0};

	while (char ch = *path++)
	{
		unsigned dist = calc_dist();
		if (dist > max_dist)
			max_dist = dist;

		switch (ch)
		{
		case 'n':
			switch (*path++)
			{
			case ',':
				// n
				++y;
				--z;
				break;
			case 'e':
				// ne
				++x;
				--z;
				break;
			case 'w':
				// nw
				++y;
				--x;
				break;
			}
			break;
		case 's':
			switch (*path++)
			{
			case ',':
				// s
				--y;
				++z;
				break;
			case 'e':
				// se
				++x;
				--y;
				break;
			case 'w':
				// sw
				--x;
				++z;
				break;
			}
			break;
		}
	}

	return { calc_dist(), max_dist };
}

TEST_CASE("main")
{
	REQUIRE(Count("ne,ne,ne,").first == 3);
	REQUIRE(Count("ne,ne,sw,sw,").first == 0);
	REQUIRE(Count("ne,ne,s,s,").first == 2);
	REQUIRE(Count("se,sw,se,sw,sw,").first == 3);

	std::ifstream ifs(INPUT);
	std::string input;
	std::getline(ifs, input);
	input.push_back(',');

	auto res = Count(input.c_str());
	std::cout << res.first << std::endl;
	std::cout << res.second << std::endl;
}
