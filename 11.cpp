#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <fstream>

unsigned Count(const char *path)
{
	// A good guide to hexagonal grids is here:
	// https://www.redblobgames.com/grids/hexagons/
	// We'll use cube coordinates (a diagonal slice of cube is hex grid too)

	int x{0}, y{0}, z{0};

	while (char ch = *path++)
	{
		switch (ch)
		{
		case 'n':
			switch (char ch2 = *path++)
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
			switch (char ch2 = *path++)
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

	return ((x < 0 ? -x : x) + (y < 0 ? -y : y) + (z < 0 ? -z : z)) / 2;
}

TEST_CASE("main")
{
	REQUIRE(Count("ne,ne,ne,") == 3);
	REQUIRE(Count("ne,ne,sw,sw,") == 0);
	REQUIRE(Count("ne,ne,s,s,") == 2);
	REQUIRE(Count("se,sw,se,sw,sw,") == 3);

	std::ifstream ifs(INPUT);
	std::string input;
	std::getline(ifs, input);
	input.push_back(',');

	std::cout << Count(input.c_str()) << std::endl;
}
